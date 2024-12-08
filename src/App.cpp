// AUI Telegram Client - example of using AUI Framework
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

#include <AUI/Logging/ALogger.h>
#include <AUI/Util/kAUI.h>
#include <view/MainView.h>
#include "App.h"
#include "view/AuthorizationView.h"
#include "util/Image.h"

static constexpr auto LOG_TAG = "App";

using namespace std::chrono_literals;

struct Stub {
    void operator()(auto& v) const {
        ALogger::info(LOG_TAG) << "Stub: " << to_string(v);
    }
};

App::App(): mUpdateTimer(_new<ATimer>(100ms)) {
    setSlotsCallsOnlyOnMyThread(true);
    connect(mUpdateTimer->fired, me::update);
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));
    initClientManager();
    mUpdateTimer->start();
}

void App::initClientManager() {
    mClientManager = std::make_unique<td::ClientManager>();
    mClientId = mClientManager->create_client_id();
    sendQuery(td::td_api::make_object<td::td_api::getOption>("version"), [](Object object) {
        td::td_api::downcast_call(*object, aui::lambda_overloaded {
            [](td::td_api::optionValueString& u) {
                ALogger::info(LOG_TAG) << "Tdlib version: " << u.value_;
            },
            [](auto&){}
        });
    });
}

void App::sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler) {
    auto query_id = ++mCurrentQueryId;
    if (handler) {
        mHandlers.emplace(query_id, std::move(handler));
    }
    mClientManager->send(mClientId, query_id, std::move(f));
}

void App::processResponse(td::ClientManager::Response response) {
    if (!response.object) {
        return;
    }
    ALOG_DEBUG(LOG_TAG) << "Response (" << response.request_id << "): " << to_string(response.object);

    if (auto c = mHandlers.contains(response.request_id)) {
        auto handler = std::move(c->second);
        mHandlers.erase(c);
        handler(std::move(response.object));
        return;
    }

    commonHandler(std::move(response.object));
}


template<typename Anything, typename Handler>
concept CanBeHandledBy = requires(Anything& anything, Handler& t) {
    t.handle(anything);
};

void App::commonHandler(td::tl::unique_ptr<td::td_api::Object> object) {
    td::td_api::downcast_call(
            *object, aui::lambda_overloaded{
                    [this](td::td_api::updateAuthorizationState &update_authorization_state) {
                        td::td_api::downcast_call(*update_authorization_state.authorization_state_, aui::lambda_overloaded {
                            [this](td::td_api::authorizationStateWaitTdlibParameters& u) {
                                auto parameters = td::td_api::make_object<td::td_api::setTdlibParameters>();
                                parameters->database_directory_ = "tdlib";
                                parameters->use_message_database_ = true;
                                parameters->use_secret_chats_ = true;
                                parameters->api_id_ = TELEGRAM_API_ID;
                                parameters->api_hash_ = AUI_PP_STRINGIZE(TELEGRAM_API_HASH);
                                parameters->system_language_code_ = "en";
                                parameters->device_model_ = "Desktop";
                                parameters->application_version_ = "1.0";
                                sendQuery(std::move(parameters));
                            },
                            [this](td::td_api::authorizationStateReady& u) {
                                auto main = _new<MainView>(sharedPtr());
                                mWindow->present(std::move(main));
                            },
                            [this](td::td_api::authorizationStateClosed& u) {
                                ui_thread {
                                    initClientManager();
                                };
                            },
                            [this](CanBeHandledBy<AuthorizationView> auto& u) {
                                auto auth = _new<AuthorizationView>(sharedPtr());
                                auth->handle(u);
                                mWindow->present(std::move(auth));
                            },
                            Stub{},
                        });
                    },
                    [this](td::td_api::updateNewChat& u) {
                        auto& chat = getChat(u.chat_->id_);
                        chat->setModel({
                            .id = u.chat_->id_,
                            .title = u.chat_->title_,
                            .previewText = MessageModel::makePreviewText(u.chat_->last_message_.get()),
                            .thumbnail = u.chat_->photo_ && u.chat_->photo_->minithumbnail_ ? _new<AImageDrawable>(util::image::from(*u.chat_->photo_->minithumbnail_)) : nullptr,
                            .inboxLastReadMessage = u.chat_->last_read_inbox_message_id_,
                            .outboxLastReadMessage = u.chat_->last_read_outbox_message_id_,
                            .unreadCount = u.chat_->unread_count_,
                        });
                        if (u.chat_->last_message_) {
                            auto lastMessage = (*chat)->getMessageOrNew(u.chat_->last_message_->id_);
                            MessageModel::populateFrom(*lastMessage, std::move(u.chat_->last_message_));
                            chat->getEditableModel().lastMessage = std::move(lastMessage);
                        }
                    },
                    [this](td::td_api::updateChatTitle& u) {
                        getChat(u.chat_id_)->setValue(&ChatModel::title, u.title_);
                    },
                    [this](td::td_api::updateChatReadInbox& u) {
                        getChat(u.chat_id_)->setValue(&ChatModel::inboxLastReadMessage, u.last_read_inbox_message_id_);
                        getChat(u.chat_id_)->setValue(&ChatModel::unreadCount, u.unread_count_);
                    },
                    [this](td::td_api::updateChatLastMessage& u) {
                        auto& chat = getChat(u.chat_id_);
                        chat->setValue(&ChatModel::previewText, MessageModel::makePreviewText(u.last_message_.get()));
                        if (u.last_message_) {
                            auto lastMessage = (*chat)->getMessageOrNew(u.last_message_->id_);
                            MessageModel::populateFrom(*lastMessage, std::move(u.last_message_));
                            chat->setValue(&ChatModel::lastMessage, std::move(lastMessage));
                        }
                    },
                    [this](td::td_api::updateUser &update_user) {
                    },
                    [this](td::td_api::updateOption& u) {
                        if (u.name_ == "my_id") {
                            mMyId = td::move_tl_object_as<td::td_api::optionValueInteger>(std::move(u.value_))->value_;
                        }
                    },
                    [this](td::td_api::updateNewMessage& u) {
                        auto msg = (*getChat(u.message_->chat_id_))->getMessageOrNew(u.message_->id_);
                        MessageModel::populateFrom(*msg, std::move(u.message_));
                    },
                    [this](td::td_api::updateMessageSendSucceeded& u) {
                        auto msg = (*getChat(u.message_->chat_id_))->getMessage(u.old_message_id_);
                        if (!msg) {
                            return;
                        }
                        msg->getEditableModel().id = u.message_->id_;
                        MessageModel::populateFrom(*msg, std::move(u.message_));
                        msg->setValue(&MessageModel::status, MessageModel::SendStatus::UNREAD);
                    },
                    Stub{}});
}

void App::update() {
    for (;;) {
        auto response = mClientManager->receive(0);
        if (!response.object) {
            return;
        }
        processResponse(std::move(response));
    }
}

void App::run() {
    mWindow = _new<MainWindow>(sharedPtr());
    mUpdateTimer->start();
    mWindow->show();
}

const _<Chat>& App::getChat(int64_t id) {
    return mChats.getOrInsert(id, [&] {
        return _new<Chat>(ChatModel { .id = id });
    });
}

