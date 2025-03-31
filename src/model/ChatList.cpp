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

#include "ChatList.h"
#include <App.h>
#include <AUI/Traits/algorithms.h>
#include "Chat.h"

void ChatList::fetchNextChats() {
    auto app = ChatList::app.lock();
    if (!app) {
        return;
    }
    app->sendQuery(td::td_api::loadChats(kindToTg(kind), 10), [](td::td_api::ok&) {});
}

ChatList::Kind ChatList::kindFromTg(td::td_api::ChatList& chatList) {
    Kind result = Main {};
    td::td_api::downcast_call(
        chatList,
        aui::lambda_overloaded {
          [&](const td::td_api::chatListMain&) { result = Main {}; },
          [&](const td::td_api::chatListArchive&) { result = Archive {}; },
          [&](const td::td_api::chatListFolder& v) { result = Folder {}; },
        });
    return result;
}

td::td_api::object_ptr<td::td_api::ChatList> ChatList::kindToTg(const ChatList::Kind& kind) {
    return std::visit(aui::lambda_overloaded {
      [](const Main&) -> td::td_api::object_ptr<td::td_api::ChatList> {
        return td::td_api::make_object<td::td_api::chatListMain>();
      },
      [](const Archive&) -> td::td_api::object_ptr<td::td_api::ChatList> {
        return td::td_api::make_object<td::td_api::chatListArchive>();
      },
      [](const Folder&) -> td::td_api::object_ptr<td::td_api::ChatList> {
        return td::td_api::make_object<td::td_api::chatListFolder>();
      },
    }, kind);
}

AVector<_<ChatList::Entry>>::const_iterator ChatList::findEntryIterator(const _<Entry>& target) {
    const auto targetOrdering = target->ordering;
    const auto it = aui::binary_search(chats->begin(), chats->end(), [targetOrdering](const AVector<_<ChatList::Entry>>::const_iterator& entry) {
      if ((*entry)->ordering == targetOrdering) {
          return aui::BinarySearchResult::MATCH;
      }
      if ((*entry)->ordering < targetOrdering) {
          return aui::BinarySearchResult::LEFT;
      }
      return aui::BinarySearchResult::RIGHT;
    });

    if (it == chats->end()) {
        return chats->end();
    }

    if (*it == target) {
        return it;
    }

    for (auto it2 = it; it2 != chats->end(); ++it2) {
        if (*it2 == target) {
            return it2;
        }
        if ((*it2)->ordering != targetOrdering) {
            break;
        }
    }

    for (auto it2 = it;; --it2) {
        if (*it2 == target) {
            return it2;
        }
        if ((*it2)->ordering != targetOrdering) {
            break;
        }
        if (it2 == chats->begin()) {
            break;
        }
    }

    return chats->end();
}
