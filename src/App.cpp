// AUIgram - example of using AUI Framework
// Copyright (C) 2024 Alex2772 and Contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//
// Created by alex2772 on 11/13/24.
//
#include <range/v3/all.hpp>

#include "App.h"

#include <AUI/Logging/ALogger.h>
#include <AUI/Util/kAUI.h>

#include <model/Chat.h>
#include <model/Message.h>
#include <view/MainView.h>
#include <view/AuthorizationView.h>
#include <util/Image.h>

static constexpr auto LOG_TAG = "App";

using namespace std::chrono_literals;

struct Stub {
    void operator()(auto& v) const { ALogger::info(LOG_TAG) << "Stub: " << to_string(v); }
};

App::App(_<MyUpdater> updater)
  : mTgUpdateTimer(_new<ATimer>(500ms)), mAutoUpdater(std::move(updater)), mAutoUpdateTimer(_new<ATimer>(30min)) {
    setSlotsCallsOnlyOnMyThread(true);
    connect(mTgUpdateTimer->fired, me::update);
    connect(mAutoUpdateTimer->fired, slot(mAutoUpdater)::checkForUpdates);
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));
    initClientManager();

    mTgUpdateTimer->start();
    mAutoUpdateTimer->start();
    mAutoUpdater->checkForUpdates();
}

void App::initClientManager() {
#if !CLIENT_DEMO
    mClientManager = std::make_unique<td::ClientManager>();
    mClientId = mClientManager->create_client_id();
    sendQuery(td::td_api::make_object<td::td_api::getOption>("version"), [](Object object) {
        td::td_api::downcast_call(
            *object,
            aui::lambda_overloaded {
              [](td::td_api::optionValueString& u) { ALogger::info(LOG_TAG) << "Tdlib version: " << u.value_; },
              [](auto&) {} });
    });
#endif
}

void App::sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler) {
#if !CLIENT_DEMO
    if (mQueryCountLastUpdate++ >= 5) {
        // Telegram is strict about using 3rdparty telegram clients. For this reason, we have to ensure that we wouldn't
        // trigger their security leading to ban of the account.
        //
        // If the application starts to SPAM with queries, we simply crash it.
        throw AException("too many queries");
    }

    auto query_id = ++mCurrentQueryId;
    if (handler) {
        mHandlers.emplace(query_id, std::move(handler));
    }
    hasPendingNetworkActivity = true;
    mClientManager->send(mClientId, query_id, std::move(f));
#endif
}

void App::processResponse(td::ClientManager::Response response) {
    if (!response.object) {
        return;
    }
//    ALOG_DEBUG(LOG_TAG) << "Response (" << response.request_id << "): " << to_string(response.object);

    if (auto c = mHandlers.contains(response.request_id)) {
        auto handler = std::move(c->second);
        mHandlers.erase(*c);
        handler(std::move(response.object));
        return;
    }

    commonHandler(std::move(response.object));
}

template <typename Anything, typename Handler>
concept CanBeHandledBy = requires(Anything& anything, Handler& t) { t.handle(anything); };

void App::commonHandler(td::tl::unique_ptr<td::td_api::Object> object) {
#if !CLIENT_DEMO
    td::td_api::downcast_call(
        *object,
        aui::lambda_overloaded {
          [this](td::td_api::updateAuthorizationState& update_authorization_state) {
              td::td_api::downcast_call(
                  *update_authorization_state.authorization_state_,
                  aui::lambda_overloaded {
                    [this](td::td_api::authorizationStateWaitTdlibParameters& u) {
                        auto parameters = td::td_api::make_object<td::td_api::setTdlibParameters>();
                        parameters->database_directory_ = "tdlib";
                        parameters->use_message_database_ = true;
                        parameters->use_secret_chats_ = true;
                        parameters->api_id_ = TELEGRAM_API_ID;
                        parameters->api_hash_ = AUI_PP_STRINGIZE(TELEGRAM_API_HASH);
                        parameters->system_language_code_ = "en";
                        parameters->device_model_ = "Desktop";
                        parameters->application_version_ = AUI_PP_STRINGIZE(AUI_CMAKE_PROJECT_VERSION);
                        sendQuery(std::move(parameters));
                    },
                    [this](td::td_api::authorizationStateReady& u) {
                        auto main = _new<MainView>(sharedPtr());
                        mWindow->present(std::move(main));
                    },
                    [this](td::td_api::authorizationStateClosed& u) {
                        ui_thread { initClientManager(); };
                    },
                    [this](CanBeHandledBy<AuthorizationView> auto& u) {
                        auto auth = _new<AuthorizationView>(sharedPtr());
                        auth->handle(u);
                        mWindow->present(std::move(auth));
                    },
                    Stub {},
                  });
          },
          [this](td::td_api::updateNewChat& u) {
              auto& chat = getChat(u.chat_->id_);
              *chat = {
                  .app = sharedPtr(),
                  .self = chat,
                  .id = u.chat_->id_,
                  .title = u.chat_->title_,
                  .previewText = Message::makePreviewText(u.chat_->last_message_.get()),
                  .thumbnail =
                      u.chat_->photo_ && u.chat_->photo_->minithumbnail_
                          ? util::image::from(*u.chat_->photo_->minithumbnail_)
                          : nullptr,
                  .inboxLastReadMessage = u.chat_->last_read_inbox_message_id_,
                  .outboxLastReadMessage = u.chat_->last_read_outbox_message_id_,
                  .viewAsTopics = u.chat_->view_as_topics_,
                  .type =
                      [&] {
                          Chat::Type result;
                          td::td_api::downcast_call(
                              *u.chat_->type_,
                              aui::lambda_overloaded {
                                [&](td::td_api::userTypeRegular&) { result = Chat::TypeUserRegular {}; },
                                [&](td::td_api::chatTypeSupergroup& i) {
                                    result = Chat::TypeSupergroup {
                                        .supergroupId = i.supergroup_id_, .isChannel = i.is_channel_
                                    };
                                },
                                Stub {},
                              });
                          return result;
                      }(),
                  .unreadCount = u.chat_->unread_count_,
              };
              if (u.chat_->last_message_) {
                  auto lastMessage = chat->getMessageOrNew(u.chat_->last_message_->id_);
                  lastMessage->populateFrom(std::move(u.chat_->last_message_));
                  chat->lastMessage = std::move(lastMessage);
              }
          },
          [this](td::td_api::updateChatTitle& u) { getChat(u.chat_id_)->title = u.title_; },
          [this](td::td_api::updateChatReadInbox& u) {
              getChat(u.chat_id_)->inboxLastReadMessage = u.last_read_inbox_message_id_;
              getChat(u.chat_id_)->unreadCount = u.unread_count_;
          },
          [this](td::td_api::updateChatReadOutbox& u) {
              getChat(u.chat_id_)->outboxLastReadMessage = u.last_read_outbox_message_id_;
          },
          [this](td::td_api::updateChatLastMessage& u) {
              auto& chat = getChat(u.chat_id_);
              chat->previewText = Message::makePreviewText(u.last_message_.get());
              if (u.last_message_) {
                  auto lastMessage = chat->getMessageOrNew(u.last_message_->id_);
                  lastMessage->populateFrom(std::move(u.last_message_));
                  chat->lastMessage = std::move(lastMessage);
              }
          },
          [this](td::td_api::updateUser& update_user) {},
          [this](td::td_api::updateOption& u) {
              if (u.name_ == "my_id") {
                  mMyId = td::move_tl_object_as<td::td_api::optionValueInteger>(std::move(u.value_))->value_;
              }
          },
          [this](td::td_api::updateNewMessage& u) {
              auto msg = getChat(u.message_->chat_id_)->getMessageOrNew(u.message_->id_);
              msg->populateFrom(std::move(u.message_));
          },
          [this](td::td_api::updateMessageSendSucceeded& u) {
              auto msg = getChat(u.message_->chat_id_)->getMessage(u.old_message_id_);
              if (!msg) {
                  return;
              }
              msg->id = u.message_->id_;
              msg->populateFrom(std::move(u.message_));
          },
          [this](td::td_api::updateConnectionState& u) {
              td::td_api::downcast_call(
                  *u.state_,
                  aui::lambda_overloaded {
                    [&](td::td_api::connectionStateReady&) { mWarmupComplete = true; },
                    [&](auto&&) { mWarmupComplete = false; },
                  });
          },
          [this](td::td_api::updateChatAddedToList& u) {
              auto kind = ChatList::kindFromTg(*u.chat_list_);
              const auto& chat = getChat(u.chat_id_);
              const auto& chatList = getChatList(kind);
              _<ChatList::Entry> entry;
              chatList->chats.writeScope() << chat->chatLists.getOrInsert(kind, [&] {
                  return entry = aui::ptr::manage(new ChatList::Entry {
                    .chat = chat,
                    .chatList = chatList,
                  });
              }).lock();
          },
          [this](td::td_api::updateChatPosition& u) {
            auto kind = ChatList::kindFromTg(*u.position_->list_);
            const auto& chat = getChat(u.chat_id_);
            if (auto it = chat->chatLists.find(kind); it != chat->chatLists.end()) {
                if (auto entry = it->second.lock()) {
                    if (auto chatList = entry->chatList.lock()) {
                        {
                            auto it2 = chatList->findEntryIterator(entry);
                            AUI_ASSERT(it2 != chatList->chats->end());
                            AUI_ASSERT(*it2 == entry);
                            chatList->chats.writeScope()->erase(it2);
                        }
                        entry->ordering = u.position_->order_;
                        auto it2 = ranges::lower_bound(*chatList->chats, entry->ordering, std::greater {},
                            [](const _<ChatList::Entry>& e) { return e->ordering; });
                        chatList->chats.writeScope()->insert(it2, std::move(entry));
                    }
                }
            }
          },
          [this](td::td_api::updateChatRemovedFromList& u) {
              auto kind = ChatList::kindFromTg(*u.chat_list_);
              const auto& chat = getChat(u.chat_id_);
              if (auto it = chat->chatLists.find(kind); it != chat->chatLists.end()) {
                  if (auto entry = it->second.lock()) {
                      if (auto chatList = entry->chatList.lock()) {
                          auto it2 = chatList->findEntryIterator(entry);
                          if (it2 != chatList->chats->end()) {
                              AUI_ASSERT(*it2 == entry);
                              chatList->chats.writeScope()->erase(it2);
                          }
                      }
                  }
                  chat->chatLists.erase(it);
              }
          },
          Stub {} });
#endif
}

void App::update() {
#if !CLIENT_DEMO
    mQueryCountLastUpdate = 0;
    for (;;) {
        auto response = mClientManager->receive(0);
        if (!response.object) {
            hasPendingNetworkActivity =
                !mHandlers.empty() || !mWarmupComplete ||
                std::any_cast<AUpdater::StatusDownloading>(&*mAutoUpdater->status) != nullptr;
            return;
        }
        processResponse(std::move(response));
    }
#endif
}

void App::run() {
    mWindow = _new<MainWindow>(sharedPtr());
    mTgUpdateTimer->start();
    mWindow->show();
}

const _<Chat>& App::getChat(int64_t id) {
    return mChats.getOrInsert(id, [&] { return aui::ptr::manage(new Chat { .id = id }); });
}
_<ChatList> App::getChatList(ChatList::Kind kind) {
    return mChatLists.getOrInsert(kind, [&] { return aui::ptr::manage(new ChatList { .kind = kind, .app = sharedPtr() }); });
}
