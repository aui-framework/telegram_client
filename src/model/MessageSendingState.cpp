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

#include "MessageSendingState.h"

MessageSendingState
MessageSendingState::make(const td::td_api::object_ptr<td::td_api::MessageSendingState>& sendingState) {
    if (sendingState != nullptr) {
        return {Pending{}};
    }
    return {None{}};
}
