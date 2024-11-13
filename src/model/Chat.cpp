//
// Created by alex2772 on 11/13/24.
//
#include "Chat.h"

const _<Message>& ChatModel::getMessage(int64_t id) const {
    return messages.getOrInsert(id, [&] {
        return _new<Message>(MessageModel{ .id = id });
    });
}
