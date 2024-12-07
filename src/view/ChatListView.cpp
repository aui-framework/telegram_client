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
#include <AUI/View/AScrollArea.h>
#include <AUI/Util/ALayoutInflater.h>
#include <AUI/View/AForEachUI.h>
#include <AUI/Model/AListModel.h>
#include <AUI/Layout/AVerticalLayout.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/ADrawableView.h>
#include "ChatListView.h"

using namespace ass;
using namespace declarative;

ChatListView::ChatListView(_<App> app) : mApp(std::move(app)) {
    setExtraStylesheet(AStylesheet {
        {
            t<ALabel>(),
            ATextOverflow::ELLIPSIS,
        },
        {
            t<AScrollArea>(),
        }
    });
    mApp->sendQuery(td::td_api::getChats(nullptr, 20), [this](td::td_api::chats& chats) {
        auto model = AListModel<_<Chat>>::fromVector(chats.chat_ids_
            | ranges::view::transform([&](int64_t id) { return mApp->getChat(id); })
            | ranges::to_vector);

        ALayoutInflater::inflate(*this, AScrollArea::Builder().withContents(
            AUI_DECLARATIVE_FOR(chat, model, AVerticalLayout) {
                return Horizontal {
                    Centered{
                        Icon{} with_style{
                            FixedSize(36_dp),
                            BorderRadius(36_dp / 2),
                            AOverflow::HIDDEN,
                        } && chat(&ChatModel::thumbnail),
                    },
                    Centered{
                        Vertical::Expanding{
                            Label{} && chat(&ChatModel::title),
                            Label{} && chat(&ChatModel::previewText),
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
