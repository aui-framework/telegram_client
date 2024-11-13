//
// Created by alex2772 on 11/13/24.
//

#include <range/v3/all.hpp>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AScrollArea.h>
#include <AUI/View/ATextField.h>
#include <AUI/View/AButton.h>
#include <AUI/Model/AListModel.h>
#include <AUI/Util/ALayoutInflater.h>
#include <AUI/View/AText.h>
#include <AUI/View/ASpacerFixed.h>
#include "ChatView.h"

using namespace ass;
using namespace declarative;

ChatView::ChatView(_<App> app, _<Chat> chat) : mApp(std::move(app)), mChat(std::move(chat)) {
    mInput = _new<ATextField>() with_style {
        Expanding(1, 0),
    };
    mInput->focus();
    setContents(Vertical::Expanding {
        Horizontal {
            Label {} && mChat(&ChatModel::title),
        },
        AScrollArea::Builder().withContents(mContentsWrap = Stacked {}).withExpanding(),
        Horizontal {
            mInput,
            Button { "Send" }.connect(&AView::clicked, me::send),
        }
    });

    setExtraStylesheet(AStylesheet {
        {
            c(".message"),
            Margin { {}, 40_dp, {}, 0 },
            Padding { 4_dp },
            BorderRadius { 12_dp },
            BackgroundSolid { AColor::BLACK.transparentize(0.90f) },
        },
        {
            c(".message_mine"),
            Margin { {}, 0, {}, 40_dp },
            BackgroundSolid { AColor::BLACK.transparentize(0.8f) },
        },
    });

    mApp->sendQuery(td::td_api::getChatHistory((*mChat)->id, (*mChat)->lastMessage ? (*(*mChat)->lastMessage)->id : 0, 0, 99, false), [this](td::td_api::messages& messages) {
        auto model = AListModel<_<Message>>::fromVector(messages.messages_
            | ranges::view::reverse
            | ranges::view::transform([&](td::td_api::object_ptr<td::td_api::message>& message) {
                auto msg = (*mChat)->getMessage(message->id_);
                msg->getEditableModel().populateFrom(std::move(message));
                return msg;
            })
            | ranges::to_vector);

        ALayoutInflater::inflate(mContentsWrap, AUI_DECLARATIVE_FOR(message, model, AVerticalLayout) {
                auto view = AText::fromString("") && (*message)(&MessageModel::text, &AText::setString);
                view << ".message";
                if ((*message)->userId == mApp->myId()) {
                    view << ".message_mine";
                }
                return view;
            }
        );
    });
}

void ChatView::send() {

}
