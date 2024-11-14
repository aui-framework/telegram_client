// AUI Telegram Client - example of using AUI Framework
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

#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AButton.h>
#include <AUI/Util/ALayoutInflater.h>
#include "MainView.h"
#include "ChatListView.h"
#include "ChatView.h"


using namespace declarative;
using namespace ass;

MainView::MainView(_<App> app) : mApp(std::move(app)) {
    setContents(Vertical {
        Horizontal {
            _new<AButton>("Logout").connect(&AView::clicked, me::logout),
        },
        Horizontal::Expanding {
            _new<ChatListView>(mApp) let {
                it with_style {
                    FixedSize{300_dp, {}},
                };

                connect(it->chatSelected, me::presentChat);
            },
            mChatWrap = Stacked::Expanding {},
        }
    });

    inflateChatPlaceholder();
}

void MainView::logout() {
    mApp->sendQuery(td::td_api::logOut{});

}

void MainView::inflateChatPlaceholder() {
    ALayoutInflater::inflate(mChatWrap, Centered {
        Label { "Select a chat to start messaging" },
    });
}

void MainView::presentChat(_<Chat> chat) {
    ALayoutInflater::inflate(mChatWrap, _new<ChatView>(mApp, std::move(chat)));
}
