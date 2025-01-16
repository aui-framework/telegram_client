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

#include <range/v3/all.hpp>

#include "ChatListView.h"

#include <AUI/Layout/AVerticalLayout.h>
#include <AUI/Model/AListModel.h>
#include <AUI/Util/ALayoutInflater.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/ADrawableView.h>
#include <AUI/View/AForEachUI.h>
#include <AUI/View/AScrollArea.h>

#include <model/Chat.h>
#include <model/Message.h>

#include "SpacerForView.h"
#include "TGIco.h"

using namespace ass;
using namespace declarative;

ChatListView::ChatListView(_<App> app) : mApp(std::move(app)) {
    setContents(Vertical {
      _new<SpacerForView>(mApp) << ".container_color",
      mContents = Centered::Expanding {},
    });
    setExtraStylesheet(AStylesheet {
      {
        t<ALabel>(),
      },
      {
        t<AScrollArea>(),
      } });

    mApp->sendQuery(td::td_api::getChats(nullptr, 20), [this](td::td_api::chats& chats) {
        auto model = AListModel<_<Chat>>::fromVector(
            chats.chat_ids_ | ranges::view::transform([&](int64_t id) { return mApp->getChat(id); }) |
            ranges::to_vector);

        setModel(model);
    });
}
void ChatListView::setModel(const _<AListModel<_<Chat>>>& model) {
    ALayoutInflater::inflate(
        mContents,
        AScrollArea::Builder()
                .withContents(
                    AUI_DECLARATIVE_FOR(chat, model, AVerticalLayout) {
                    return Horizontal {
                        Centered {
                          Icon {} with_style {
                            FixedSize(36_dp),
                            BorderRadius { 36_dp / 2.f },
                            AOverflow::HIDDEN,
                          } & chat->thumbnail,
                        },
                        Centered::Expanding {
                          Vertical::Expanding {
                            Horizontal {
                              Label {} with_style { ATextOverflow::ELLIPSIS, Expanding() } && chat->title,
                              Centered {} & chat->lastMessage.readProjected([](const _<Message>& msg) -> _<AView> {
                                  if (msg == nullptr) {
                                      return nullptr;
                                  }
                                  return Horizontal {
                                      _new<TGIco>() with_style { FontSize{10_pt} } << ".accent_textcolor" &
                                          msg->statusIcon > &TGIco::setIconHideIfNone,
                                      Label {} with_style { Opacity(0.6f), Margin(0) } &
                                          msg->date.readProjected(Message::dateFmt),
                                  } with_style {
                                      LayoutSpacing{2_dp},
                                  };
                              }),
                            },
                            Horizontal {
                              Label {} with_style {
                                Expanding(),
                                ATextOverflow::ELLIPSIS,
                                Opacity(0.6f),
                              } && chat->previewText,
                              Centered {
                                Label {} with_style {
                                  MinSize { 16_dp - 4_dp * 2.f, 16_dp },
                                  FontSize { 10_dp },
                                  BorderRadius { 16_dp / 2.f },
                                  Padding { 0, 4_dp },
                                  Margin { 0 },
                                  ATextAlign::CENTER,
                                } << ".unread_count" &
                                    chat->unreadCount.readProjected(AString::number<int>) &
                                    chat->unreadCount.readProjected([](int count) {
                                        return count > 0 ? Visibility::VISIBLE : Visibility::GONE;
                                    }) > &AView::visibility,
                              },
                            } with_style {
                              LayoutSpacing{2_dp},
                            },
                          },
                        },
                    }
                        .connect(&AView::clicked, this, [this, chat] { emit chatSelected(chat); }) with_style {
                            Padding { 5_dp, 6_dp },
                            Margin { 0 },
                        };
                })
                .build()
            << ".container_color");
}
