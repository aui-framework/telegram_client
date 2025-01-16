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

#include <td/telegram/td_api.hpp>
#include "Message.h"
#include "view/TGIco.h"
#include "util/Image.h"

using namespace std::chrono;
using namespace std::chrono_literals;

AString Message::makePreviewText(td::td_api::message* message) {
    AString result = "message";

    if (message) {
        td::td_api::downcast_call(*message->content_, aui::lambda_overloaded{
                [&](td::td_api::messageAudio& u) {
                    result = "Audio";
                },
                [&](td::td_api::messageText& u) {
                    result = AString(u.text_->text_).replaceAll('\n', ' ');
                },
                [&](td::td_api::messagePhoto& u) {
                    result = AString(u.caption_->text_).replaceAll('\n', ' ');
                },
                [](auto&) {},
        });
    }

    return result;
}

void Message::populateFrom(td::td_api::object_ptr<td::td_api::message> message) {
    chatId = message->chat_id_;
    content = makeContent(message->content_);
    date = std::chrono::system_clock::from_time_t(message->date_);
    td::td_api::downcast_call(*message->sender_id_, aui::lambda_overloaded {
        [&](td::td_api::messageSenderUser& u) {
            userId = u.user_id_;
        },
        [](auto&) {},
    });
    isOutgoing = message->is_outgoing_;
    sendingState = MessageSendingState::make(message->sending_state_);
}

Message::Content Message::makeContent(td::td_api::object_ptr<td::td_api::MessageContent>& content) {
    Content result;
    td::td_api::downcast_call(*content, aui::lambda_overloaded{
            [&](td::td_api::messageText& u) {
                result.text = u.text_->text_;
            },
            [&](td::td_api::messagePhoto& u) {
                result.text = u.caption_->text_;
                auto& size = u.photo_->sizes_.front();
                result.photo = Content::Photo {
                    .drawable = util::image::from(*u.photo_->minithumbnail_),
                    .size = { size->width_, size->height_ },
                };
            },
            [](auto&) {},
    });
    return result;
}

APropertyPrecomputed<TGIco::Icon>::Factory Message::statusIconProperty() {
    return [&] {
        if (std::holds_alternative<MessageSendingState::Pending>(sendingState->value)) {
            return TGIco::CHECKMARK5_CLOCK;
        }
        auto chat = this->chat.lock();
        if (!chat) {
            return TGIco::XYI_ZNAET;
        }
        auto app = chat->app.lock();
        if (!app) {
            return TGIco::XYI_ZNAET;
        }
        if (app->myId() != userId) {
            return TGIco::NONE;
        }
        if (chat->outboxLastReadMessage >= id) {
            return TGIco::READ;
        }
        return TGIco::UNREAD;
    };
}

AString Message::dateShortFmt(std::chrono::system_clock::time_point time) {
    return "{:%H:%M}"_format(time);
}
AString Message::dateFmt(std::chrono::system_clock::time_point time) {
    const auto NOW = system_clock::now();
    if (NOW - time <= 24h) {
        return "{:%R}"_format(time); // 12:00, 03:43
    }
    if (NOW - time <= days(7)) {
        return "{:%a}"_format(time); // Fri, Wed
    }
    if (std::chrono::floor<years>(NOW) != std::chrono::floor<years>(time)) {
        return "{:%D}"_format(time); // 12/30/2024
    }
    return "{:%m/%d}"_format(time); // 12/30
}
