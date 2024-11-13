#pragma once

#include <AUI/Util/ADataBinding.h>
#include <AUI/Common/AMap.h>
#include "Message.h"

struct ChatModel {
    int64_t id;
    AString title;
    AString previewText;
    _<Message> lastMessage;

    mutable AMap<int64_t, _<Message>> messages;

    const _<Message>& getMessage(int64_t id) const;
};

using Chat = ADataBinding<ChatModel>;