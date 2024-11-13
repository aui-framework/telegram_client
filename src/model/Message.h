#pragma once

#include <AUI/Util/ADataBinding.h>
#include <td/telegram/td_api.h>

struct MessageModel {
    int64_t id;
    AString text;
    int64_t userId = 0;

    static AString makePreviewText(td::td_api::message* message);
    void populateFrom(td::td_api::object_ptr<td::td_api::message> message);

};

using Message = ADataBinding<MessageModel>;
