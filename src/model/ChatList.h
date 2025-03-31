// AUI Telegram Client - example of using AUI Framework
// Copyright (C) 2025 Alex2772 and Contributors
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

#include <AUI/Common/AObject.h>
#include <AUI/Common/AProperty.h>
#include <AUI/Model/AListModel.h>
#include <td/telegram/td_api.h>

struct Chat;
class App;

struct ChatList {
    struct Main {
        auto operator<=>(const Main&) const noexcept = default;
    };
    struct Archive {
        auto operator<=>(const Archive&) const noexcept = default;
    };
    struct Folder {
        auto operator<=>(const Folder&) const noexcept = default;
    };
    using Kind = std::variant<Main, Archive, Folder>;

    Kind kind = Main{};

    static Kind kindFromTg(td::td_api::ChatList& chatList);
    static td::td_api::object_ptr<td::td_api::ChatList> kindToTg(const Kind& kind);

    _weak<App> app;

    struct Entry {
        int64_t ordering = 0;
        _<Chat> chat;
        _weak<ChatList> chatList;
    };

    AProperty<AVector<_<Entry>>> chats;

    AVector<_<Entry>>::const_iterator findEntryIterator(const _<Entry>& target);

    void fetchNextChats();
};

template<>
struct std::hash<ChatList::Main> {
    size_t operator()(const ChatList::Main& t) const noexcept
    {
        return 0;
    }
};

template<>
struct std::hash<ChatList::Archive> {
    size_t operator()(const ChatList::Archive& t) const noexcept
    {
        return 0;
    }
};

template<>
struct std::hash<ChatList::Folder> {
    size_t operator()(const ChatList::Folder& t) const noexcept
    {
        return 0;
    }
};
