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
#include <range/v3/all.hpp>
#include "Chat.h"
#include "Message.h"

const _<Message>& Chat::getMessageOrNew(int64_t id) {
    auto l = ranges::lower_bound(*messages, id, std::less<>{}, [](const _<Message>& msg) {  return msg->id; });
    if (l != messages->end()) {
        if ((*l)->id == id) {
            return *l;
        }
    }
    return *messages.writeScope()->insert(l, aui::ptr::manage(new Message{ .chat = self, .id = id }));
}

_<Message> Chat::getMessage(int64_t id) const {
    auto l = ranges::lower_bound(*messages, id, std::less<>{}, [](const _<Message>& msg) {  return msg->id; });
    if (l != messages->end()) {
        if ((*l)->id == id) {
            return *l;
        }
    }
    return nullptr;
}

void Chat::updateChatListOrder(ChatList::Kind kind, int64_t ordering) {
    if (auto it = chatLists.find(kind); it != chatLists.end()) {
        if (auto entry = it->second.lock()) {
            if (auto chatList = entry->chatList.lock()) {
                auto chats = chatList->chats.writeScope();
                {
                    auto it2 = chatList->findEntryIterator(entry);
                    AUI_ASSERT(it2 != chats->end());
                    AUI_ASSERT(*it2 == entry);
                    chats->erase(it2);
                }
                entry->ordering = ordering;
                auto it2 = ranges::lower_bound(*chats, entry->ordering, std::greater {},
                                               [](const _<ChatList::Entry>& e) { return e->ordering; });
                chats->insert(it2, std::move(entry));
            }
        }
    }
}
