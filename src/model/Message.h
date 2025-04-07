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
#include <td/telegram/td_api.h>
#include <AUI/Image/IDrawable.h>
#include "MessageSendingState.h"
#include "Chat.h"
#include "view/TGIco.h"

struct Message {
    _weak<Chat> chat;
    int64_t id;
    int64_t chatId{};
    AProperty<int64_t> userId = 0;
    AProperty<std::chrono::system_clock::time_point> date;

    struct Content {
        AString text;
        struct Photo {
            _<IDrawable> drawable;
            glm::ivec2 size;
        };
        AOptional<Photo> photo;
    };
    AProperty<Content> content;
    bool isOutgoing{};

    AProperty<MessageSendingState> sendingState;
    APropertyPrecomputed<TGIco::Icon> statusIcon = statusIconProperty();

    static AString makePreviewText(td::td_api::message* message);
    void populateFrom(App& app, td::td_api::object_ptr<td::td_api::message> message);
    static Content makeContent(App& app, td::td_api::object_ptr<td::td_api::MessageContent>& content);

    static AString dateShortFmt(std::chrono::system_clock::time_point time);;

    static AString dateFmt(std::chrono::system_clock::time_point time);;

private:
    APropertyPrecomputed<TGIco::Icon>::Factory statusIconProperty();
};
