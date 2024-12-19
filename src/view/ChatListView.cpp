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


#include "SpacerForView.h"

using namespace ass;
using namespace declarative;

ChatListView::ChatListView(_<App> app) : mApp(std::move(app)) {
    _<AViewContainer> contents = Centered::Expanding {};
    setContents(Vertical {
      _new<SpacerForView>(mApp) << ".container_color",
      contents,
    });
    setExtraStylesheet(AStylesheet {
        {
            t<ALabel>(),
        },
        {
            t<AScrollArea>(),
        }
    });

    mApp->sendQuery(td::td_api::getChats(nullptr, 20), [this, contents](td::td_api::chats& chats) {
        auto model = AListModel<_<Chat>>::fromVector(chats.chat_ids_
            | ranges::view::transform([&](int64_t id) { return mApp->getChat(id); })
            | ranges::to_vector);

        ALayoutInflater::inflate(contents, AScrollArea::Builder().withContents(
            AUI_DECLARATIVE_FOR(chat, model, AVerticalLayout) {
                return Horizontal {
                    Centered{
                        Icon{} with_style{
                            FixedSize(36_dp),
                            BorderRadius{36_dp / 2.f},
                            AOverflow::HIDDEN,
                        } && chat(&ChatModel::thumbnail),
                    },
                    Centered::Expanding {
                        Vertical::Expanding{
                            Label{} with_style { ATextOverflow::ELLIPSIS }&& chat(&ChatModel::title),
                            Horizontal {
                                Label{} with_style { Expanding(), ATextOverflow::ELLIPSIS } && chat(&ChatModel::previewText),
                                Label{} with_style {
                                    MinSize { 16_dp - 4_dp * 2.f, 16_dp },
                                    FontSize { 8_pt },
                                    BorderRadius { 16_dp / 2.f },
                                    Padding { 0, 4_dp },
                                    ATextAlign::CENTER,
                                } << ".unread_count" && chat(&ChatModel::unreadCount, AString::number<int>)
                                                     && chat(&ChatModel::unreadCount, [](int count) {
                                                         return count > 0 ? Visibility::VISIBLE : Visibility::GONE;
                                                     }),
                            },
                        }
                    },
                }.connect(&AView::clicked, this, [this, chat] {
                    emit chatSelected(chat);
                }) with_style {
                    Padding { 5_dp, 6_dp },
                    Margin { 0 },
                };
            }
        ).build() << ".container_color");
    });
}
