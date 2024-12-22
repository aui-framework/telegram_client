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

#include <td/telegram/td_api.hpp>
#include "Message.h"
#include "view/TGIco.h"
#include "util/Image.h"

AString MessageModel::makePreviewText(td::td_api::message* message) {
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

void MessageModel::populateFrom(ADataBinding<MessageModel>& self, td::td_api::object_ptr<td::td_api::message> message) {
    self.setValue(&MessageModel::content, makeContent(message->content_));
    self.setValue(&MessageModel::date, std::chrono::system_clock::from_time_t(message->date_));
    td::td_api::downcast_call(*message->sender_id_, aui::lambda_overloaded {
        [&](td::td_api::messageSenderUser& u) {
            self.setValue(&MessageModel::userId, u.user_id_);
        },
        [](auto&) {},
    });
    self.setValue(&MessageModel::isOutgoing, message->is_outgoing_);
    if (self->isOutgoing) {
        if (message->sending_state_ != nullptr) {
            self.setValue(&MessageModel::status, MessageModel::SendStatus::SENDING);
        } else {
            self.setValue(&MessageModel::status, MessageModel::SendStatus::UNREAD);
        }
    }
}

MessageModel::Content MessageModel::makeContent(td::td_api::object_ptr<td::td_api::MessageContent>& content) {
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

AString MessageModel::sendStatusToIcon(MessageModel::SendStatus status) {
    switch (status) {
        case SendStatus::NONE: return "";
        case SendStatus::SENDING:
            return TGIco::CHECKMARK5_CLOCK;
        case SendStatus::UNREAD:
            return TGIco::UNREAD;
        case SendStatus::READ:
            return TGIco::READ;
    }
    return "";
}

