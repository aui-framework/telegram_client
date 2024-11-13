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
                    result = u.text_->text_;
                },
                [](auto&) {},
        });
    }

    return result;
}

void MessageModel::populateFrom(td::td_api::object_ptr<td::td_api::message> message) {
    text = makePreviewText(message.get());
    td::td_api::downcast_call(*message->sender_id_, aui::lambda_overloaded {
        [&](td::td_api::messageSenderUser& u) {
            userId = u.user_id_;
        },
        [](auto&) {},
    });
}
