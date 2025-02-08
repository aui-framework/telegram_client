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

#pragma once

#include <AUI/Util/ADataBinding.h>
#include <AUI/Common/AMap.h>
#include <AUI/Image/IDrawable.h>
#include <AUI/Model/AListModel.h>
#include "MessageSendingState.h"
#include "ChatList.h"

class App;
struct Message;
struct MessageSponsored;

struct Chat {
    _weak<App> app;
    _weak<Chat> self;
    int64_t id;
    AProperty<AString> title;
    AProperty<AString> previewText;
    AProperty<_<Message>> lastMessage;
    AProperty<_<IDrawable>> thumbnail;
    AProperty<int64_t> inboxLastReadMessage;
    AProperty<int64_t> outboxLastReadMessage;
    AProperty<bool> viewAsTopics;

    AUnorderedMap<ChatList::Kind, _weak<ChatList::Entry>> chatLists;

    struct TypeUserRegular {
        bool operator==(const TypeUserRegular&) const = default;
    };
    struct TypeSupergroup {
        int64_t supergroupId;
        bool isChannel;
        bool operator==(const TypeSupergroup&) const = default;
    };
    using Type = std::variant<TypeUserRegular, TypeSupergroup>;
    AProperty<Type> type;
    AProperty<int> unreadCount;

    _<AListModel<_<Message>>> messages = _new<AListModel<_<Message>>>();
    _<AListModel<_<MessageSponsored>>> sponsoredMessages = _new<AListModel<_<MessageSponsored>>>();

    const _<Message>& getMessageOrNew(int64_t id) const;
    _<Message> getMessage(int64_t id) const;
};
