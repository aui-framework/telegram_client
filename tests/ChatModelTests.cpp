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
// Created by alex2772 on 11/15/24.
//

#include <gmock/gmock.h>
#include "model/Chat.h"
#include "model/Message.h"

TEST(ChatModel, Insert) {
    auto chat = aui::ptr::manage(new Chat{});
    auto msg1 = chat->getMessageOrNew(1);
    chat->getMessageOrNew(0);
    chat->getMessageOrNew(2);
    EXPECT_EQ(msg1->id, 1);
    auto msg2 = chat->getMessageOrNew(1);
}