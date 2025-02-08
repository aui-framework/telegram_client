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

#include "MainView.h"

#include <AUI/Util/ALayoutInflater.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AButton.h>
#include <AUI/View/ASpacerFixed.h>
#include <AUI/View/ASplitter.h>
#include <view/TGIco.h>

#include "ChatListView.h"
#include "ChatView.h"

using namespace declarative;
using namespace ass;

MainView::MainView(_<App> app) : mApp(std::move(app)) {
    setContents(Vertical {
      //      Horizontal {
      //        _new<AButton>("Logout").connect(&AView::clicked, me::logout),
      //        _new<TGIco>(TGIco::CHECKMARK5_CLOCK),
      //      },
      ASplitter::Horizontal()
          .withItems({
            Vertical {
              mChatListView =
                  _new<ChatListView>(mApp->getChatList(ChatList::Main{})) let {
                      it with_style { MinSize { 250_dp }, Expanding() };
                      connect(it->chatSelected, me::presentChat);
                  },
              CustomLayout {} & mApp->autoUpdater()->status.readProjected([this](const std::any& status) -> _<AView> {
                  if (std::any_cast<AUpdater::StatusWaitingForApplyAndRestart>(&status)) {
                      return Button { Centered::Expanding { Horizontal { _new<TGIco>(TGIco::Icon::UPDATE), Label { "Apply update and restart" } } } }.connect(&AView::clicked, slot(mApp->autoUpdater())::applyUpdateAndRestart) with_style {
                          TextColor(0xffffff_rgb),
                          BackgroundGradient(0x1b97bd_rgb, 0x13b252_rgb, 132_deg),
                          Border(1_px, 0x70ffffff_argb),
                          Padding(12_dp, {}),
                          Margin(0),
                          BorderRadius(0),
                          ACursor::POINTER,
                      };
                  }
                  return nullptr;
              })
            },
            mChatWrap = Stacked::Expanding {} with_style { MinSize { 300_dp } },
          })
          .build() with_style {
            LayoutSpacing{1_px},
            Expanding(),
          },
    });

    inflateChatPlaceholder();
}

void MainView::logout() { mApp->sendQuery(td::td_api::logOut {}); }

void MainView::inflateChatPlaceholder() {
    ALayoutInflater::inflate(
        mChatWrap,
        Centered {
          Vertical {
            Label { "Select a chat to start messaging" } with_style {
              FixedSize({}, 22_dp),
              Padding { {}, 12_dp },
              BorderRadius{22_dp / 2.f},
            } << ".container_color",
          },
        });
}

void MainView::presentChat(_<Chat> chat) { ALayoutInflater::inflate(mChatWrap, _new<ChatView>(mApp, std::move(chat))); }
