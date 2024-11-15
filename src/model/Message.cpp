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

AString MessageModel::makePreviewText(td::td_api::message* message) {
    AString result = "message";

    if (message) {
        td::td_api::downcast_call(*message->content_, aui::lambda_overloaded{
                [&](td::td_api::messageAudio& u) {
                    result = "Audio";
                },
                [&](td::td_api::messageText& u) {
                    result = u.text_->text_.substr(0, u.text_->text_.find('\n'));
                },
                [](auto&) {},
        });
    }

    return result;
}

void MessageModel::populateFrom(ADataBinding<MessageModel>& self, td::td_api::object_ptr<td::td_api::message> message) {
    self.setValue(&MessageModel::text, makePreviewText(message.get()));
    self.setValue(&MessageModel::date, std::chrono::system_clock::from_time_t(message->date_));
    td::td_api::downcast_call(*message->sender_id_, aui::lambda_overloaded {
        [&](td::td_api::messageSenderUser& u) {
            self.setValue(&MessageModel::userId, u.user_id_);
        },
        [](auto&) {},
    });
}
