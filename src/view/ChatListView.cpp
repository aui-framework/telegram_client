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
            ScrollbarAppearance { ScrollbarAppearance::VISIBLE, ScrollbarAppearance::GONE },
        }
    });
    mApp->sendQuery(td::td_api::getChats(nullptr, 20), [this](td::td_api::chats& chats) {
        auto model = AListModel<_<Chat>>::fromVector(chats.chat_ids_
            | ranges::view::transform([&](int64_t id) { return mApp->getChat(id); })
            | ranges::to_vector);

        ALayoutInflater::inflate(*this, AScrollArea::Builder().withContents(
            AUI_DECLARATIVE_FOR(chat, model, AVerticalLayout) {
                return Horizontal {
                    Label { "(icon)" },
                    Vertical {
                        Label {} && chat(&ChatModel::title),
                        Label {} && chat(&ChatModel::previewText),
                    }
                }.connect(&AView::clicked, this, [this, chat] {
                    emit chatSelected(chat);
                });
            }
        ));
    });
}
