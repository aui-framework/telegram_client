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
                    FixedSize{200_dp, {}},
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
