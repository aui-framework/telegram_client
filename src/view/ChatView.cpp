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
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AScrollArea.h>
#include <AUI/View/ATextField.h>
#include <AUI/View/AButton.h>
#include <AUI/Model/AListModel.h>
#include <AUI/Util/ALayoutInflater.h>
#include <AUI/View/AText.h>
#include <AUI/View/ASpacerFixed.h>
#include <AUI/View/ADrawableView.h>
#include "ChatView.h"

using namespace ass;
using namespace declarative;

ChatView::ChatView(_<App> app, _<Chat> chat) : mApp(std::move(app)), mChat(std::move(chat)) {
    mInput = _new<ATextField>() with_style {
        Expanding(1, 0),
    };
    mInput->focus();
    setContents(Vertical::Expanding {
        Horizontal {
            Centered{
                Icon{} with_style {
                    FixedSize(32_dp),
                    BorderRadius(16_dp),
                    AOverflow::HIDDEN,
                } && mChat(&ChatModel::thumbnail),
            },
            Vertical::Expanding {
                Label {} && mChat(&ChatModel::title),
                Label { "last seen" },
            },
        } with_style {
            Padding { 4_dp },
            BorderLeft { 1_px, AColor::GRAY.opacify(0.3f) },
            BorderBottom { 1_px, AColor::GRAY.opacify(0.3f) },
            Margin { {}, {}, 1_px, 1_px }
        },
        AScrollArea::Builder().withContents(mContentsWrap = Stacked {}).withExpanding().build() let {
            it->verticalScrollbar()->setStickToEnd(true);
        },
        Horizontal {
            mInput,
            Button { "Send" }.connect(&AView::clicked, me::send),
        }
    });

    setExtraStylesheet(AStylesheet {
        {
            c(".message"),
            Padding { 6_dp, 6_dp, 3_dp },
            BorderRadius { 12_dp },
            BackgroundSolid { AColor::BLACK.transparentize(0.90f) },
//            ATextAlign::JUSTIFY,
        },
        {
            c(".message_mine"),
            BackgroundSolid { AColor::BLACK.transparentize(0.8f) },
        },
    });

    mApp->sendQuery(td::td_api::getChatHistory((*mChat)->id, (*mChat)->lastMessage ? (*(*mChat)->lastMessage)->id : 0, 0, 99, false), [this](td::td_api::messages& messages) {
        for (auto& message : messages.messages_) {
            auto msg = (*mChat)->getMessage(message->id_);
            MessageModel::populateFrom(*msg, std::move(message));
        }

        ALayoutInflater::inflate(mContentsWrap, AUI_DECLARATIVE_FOR(message, (*mChat)->messages, AVerticalLayout) {
            auto view = AText::fromString("") with_style {
                MaxSize { 400_dp, {} },
                Expanding(0, 0),
            } && (*message)(&MessageModel::text, [&message = *message](AText& view, const AString& data) {
                view.setItems({
                    data,
                    Horizontal {
                        _new<ALabel>() with_style { Margin { 0 }, Padding { 0 }, FontSize { 8_pt }, Opacity { 0.5f } }
                            && message(&MessageModel::date, [](std::chrono::system_clock::time_point time) {
                                 return "{:%H:%M}"_format(time);
                               }),
                    } with_style {
                        Margin { 4_dp, {}, {}, 4_dp, },
                        AFloat::RIGHT,
                    },
                });
            });
            view << ".message";

            // hack: force recall AUI_DECLARATIVE_FOR clause when userId is updated.
            message->addObserverNoInitialCall(&MessageModel::userId, [&chat = *mChat, msgId = (*message)->id](int64_t userId) {
                auto it = ranges::find(chat->messages, msgId, [](const _<Message>& msg) { return (*msg)->id; });
                if (it != chat->messages.end()) {
                    chat->messages->invalidate(it);
                }
            });

            const bool mine = (*message)->userId == mApp->myId();
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
        });
    });
}

void ChatView::send() {

}
