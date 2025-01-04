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
#include <range/v3/all.hpp>
#include "ChatView.h"

#include <AUI/Model/AListModel.h>
#include <AUI/Util/ALayoutInflater.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AButton.h>
#include <AUI/View/ADrawableView.h>
#include <AUI/View/AScrollArea.h>
#include <AUI/View/ASpacerFixed.h>
#include <AUI/View/AText.h>
#include <AUI/View/ATextField.h>


#include "SpacerForView.h"
#include "TGIco.h"
#include <model/MessageSponsored.h>

using namespace ass;
using namespace declarative;

template <typename MessageModelT>
_<AViewContainer> ChatView::makeMessage(const _<MessageModelT>& message) {
    auto text = AText::fromString("") with_style {
        MaxSize { 400_dp, {} },
        Expanding(0, 0),
        Padding { 7_dp, 9_dp, 4_dp },
        Margin { 0 },
    } & message->content > [&message = *message](AText& view, const Message::Content& data) {
        view.setItems({
          data.text,
          Horizontal {
            [&]() -> aui::ui_building::ViewGroup {
                if constexpr (requires { MessageModelT::date; }) {
                    return {
                        Horizontal {
                          _new<ALabel>() with_style { Margin { 0 }, Padding { 0 }, FontSize { 12_dp } } &
                              message.date.readProjected(Message::dateShortFmt),
                        } with_style {
                          Opacity { 0.3f },
                        },
                        _new<TGIco>() with_style { FontSize { 14_dp }, FixedSize { {}, 14_dp } }
                                << ".status" << ".accent_textcolor" & message.statusIcon > &TGIco::setIconHideIfNone,
                    };
                }
                if constexpr (requires { MessageModelT::isRecommended; }) {
                    return { _new<ALabel>(message.isRecommended ? "recommended" : "sponsored") with_style {
                      Margin { 0 }, Padding { 0 }, FontSize { 12_dp }, Opacity { 0.3f } } };
                }
                return {};
            }(),
          } with_style {
            Margin { 3_dp, {}, -2_dp, 4_dp },
            AFloat::RIGHT,
          },
        });
    };
    _<AViewContainer> view = Vertical {
        text,
    } << ".message";
    if (message->content->photo) {
        view->addView(
            0,
            _new<ADrawableView>(message->content->photo->drawable) with_style {
              MinSize(AMetric(message->content->photo->size.x, AMetric::T_DP),
                      AMetric(message->content->photo->size.y, AMetric::T_DP)),
              Expanding(1, 1),
            });
        view << ".message_has_photo";
    }

    if (mChat->inboxLastReadMessage < message->id) {
        // let's track visibility of this message in order to update read status.
        connect(mScrollArea->scrolled, [this, messageViewWeak = view->weakPtr(), messageId = message->id] {
            auto messageView = messageViewWeak.lock();
            if (!messageView) {
                AObject::disconnect();
                return;
            }
            auto rectScrollArea =
                ARect<int>::fromTopLeftPositionAndSize(mScrollArea->getPositionInWindow(), mScrollArea->getSize());
            auto rectMessage =
                ARect<int>::fromTopLeftPositionAndSize(messageView->getPositionInWindow(), messageView->getSize());
            if (ranges::all_of(rectMessage.vertices(), [&](auto point) {
                    return rectScrollArea.isIntersects(point);
                })) {
                mChat->inboxLastReadMessage = glm::max(*mChat->inboxLastReadMessage, messageId);
                mReadMessagesBatch << messageId;
                AObject::disconnect();
                ui_thread {
                    auto messagesToMarkViewed = std::exchange(mReadMessagesBatch, {});
                    if (messagesToMarkViewed.empty()) {
                        return;
                    }
                    td::td_api::viewMessages viewMessages;
                    viewMessages.chat_id_ = mChat->id;
                    viewMessages.message_ids_ = std::move(messagesToMarkViewed);
                    mApp->sendQuery(std::move(viewMessages));
                };
            }
        });
    }
    return view;
}

ChatView::ChatView(_<App> app, _<Chat> chat) : mApp(std::move(app)), mChat(std::move(chat)) {
    mInput = _new<ATextArea>() let {
        it->focus();
        connect(it->actionButtonPressed, me::send);
    };

    {
        td::td_api::openChat query;
        query.chat_id_ = mChat->id;
        mApp->sendQuery(std::move(query));
    }

    _<AViewContainer> title = Vertical {
        _new<SpacerForView>(mApp),
        Horizontal {
          Centered {
            Icon {} with_style {
              FixedSize(32_dp),
              BorderRadius(16_dp),
              AOverflow::HIDDEN,
            } & mChat->thumbnail,
          },
          Vertical::Expanding {
            Label {} & mChat->title,
            [&] {
                return std::visit(
                    aui::lambda_overloaded {
                      [](const Chat::TypeUserRegular& regular) -> _<AView> { return Label { "last seen <TODO>" }; },
                      [](const Chat::TypeSupergroup& supergroup) -> _<AView> { return Label { "Supergroup" }; },
                    },
                    *mChat->type);
            }(),
          },
        } with_style {
          Padding { 4_dp },
          BorderBottom { 1_px, AColor::GRAY.opacify(0.3f) },
          Margin { {}, {}, 1_px, 1_px },
        },
    } << ".container_color_blur";
    setContents(
        Stacked::Expanding {
          Vertical::Expanding {
            _new<SpacerForView>(title),
            mScrollArea =
                AScrollArea::Builder().withContents(mContentsWrap = Stacked {}).withExpanding().build() let {
                    it->setStickToEnd(true);
                    it with_style {
                        MinSize(200_dp),
                    };
                    it->setExtraStylesheet({ {
                      t<AScrollAreaViewport>(),
                      AOverflow::VISIBLE,
                    } });
                    it << ".container_chat_background_color";
                },
            Horizontal {
              AScrollArea::Builder().withContents(mInput).build() with_style {
                Expanding(1, 0),
                MaxSize { {}, 300_dp },
                Padding { 8_dp },
              },
              Vertical {
                SpacerExpanding(),
                Button { "Send" }.connect(&AView::clicked, me::send),
              } with_style { Padding(8_dp) },
            } << ".container_color_blur",
          },
          Vertical::Expanding {
            title,
          },
        } with_style {
          AOverflow::HIDDEN,
        }
        << ".container_color");


    ALayoutInflater::inflate(
        mContentsWrap,
        Vertical {
          AUI_DECLARATIVE_FOR(message, mChat->messages, AVerticalLayout) {
              auto view = makeMessage(message);

              // hack: force recall AUI_DECLARATIVE_FOR clause when userId is updated.
              connect(message->userId.changed, [chat = mChat, msgId = message->id](int64_t userId) {
                  auto it = ranges::find(chat->messages, msgId, [](const _<Message>& msg) {
                      return msg->id;
                  });
                  if (it != chat->messages.end()) {
                      chat->messages->invalidate(it);
                  }
              });

              const bool mine = message->userId == mApp->myId();
              if (mine) {
                  view << ".message_mine";
                  return Horizontal {
                      SpacerExpanding(),
                      view,
                  };
              }
              return Horizontal {
                  view,
              };
          },
          AUI_DECLARATIVE_FOR(message, mChat->sponsoredMessages, AVerticalLayout) {
              auto view = makeMessage(message);
              return Horizontal {
                  view,
              };
          },
        });

    mApp->sendQuery(
        td::td_api::getChatHistory(
            mChat->id, mChat->lastMessage.value() ? mChat->lastMessage.value()->id : 0, 0, 99, false),
        [this](td::td_api::messages& messages) {
            for (auto& message : messages.messages_) {
                auto msg = mChat->getMessageOrNew(message->id_);
                msg->populateFrom(std::move(message));
            }

        });


    if (auto superGroup = std::get_if<Chat::TypeSupergroup>(&*mChat->type)) {
        if (superGroup->isChannel) {
            mApp->sendQuery(
                td::td_api::getChatSponsoredMessages(mChat->id), [&](td::td_api::sponsoredMessages& messages) {
                    mChat->sponsoredMessages->clear();
                    for (auto& msg : messages.messages_) {
                        mChat->sponsoredMessages << aui::ptr::manage(new MessageSponsored {
                          .id = msg->message_id_,
                          .content = Message::makeContent(msg->content_),
                          .isRecommended = msg->is_recommended_,
                        });
                    }
                });
        }
    }
}

void ChatView::send() {
    auto text = *mInput->text();
    mInput->setText("");
    auto msg = td::td_api::make_object<td::td_api::sendMessage>();
    msg->chat_id_ = mChat->id;
    msg->input_message_content_ = [&] {
        auto content = td::td_api::make_object<td::td_api::inputMessageText>();
        content->text_ = [&] {
            auto t = td::td_api::make_object<td::td_api::formattedText>();
            t->text_ = text.toStdString();
            return t;
        }();
        return content;
    }();
    mApp->sendQuery(std::move(msg), [&](td::td_api::message&) { mScrollArea->verticalScrollbar()->scrollToEnd(); });
}

ChatView::~ChatView() {
    td::td_api::closeChat query;
    query.chat_id_ = mChat->id;
    mApp->sendQuery(std::move(query));
}
