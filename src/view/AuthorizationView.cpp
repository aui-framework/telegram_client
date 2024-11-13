//
// Created by alex2772 on 11/13/24.
//

#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AText.h>
#include <AUI/View/ATextField.h>
#include <AUI/View/AButton.h>
#include "AuthorizationView.h"

using namespace ass;
using namespace declarative;

AuthorizationView::AuthorizationView(_<App> app): mApp(std::move(app)) {
    setExtraStylesheet(AStylesheet {
        {
            ass::c(".title"),
            FontSize { 18_pt },
        }
    });
}

void AuthorizationView::handle(td::td_api::authorizationStateWaitPhoneNumber& state) {
    auto countryCode = _new<ATextField>() with_style {
        FixedSize { 30_dp },
    };
    countryCode->setText("+7");
    auto number = _new<ATextField>() with_style {
        Expanding { 1, 0 },
    };

    setContents(Centered { Vertical {
        Label { "Your Phone Number" } << ".title",
        AText::fromString("Please confirm your country code and enter your phone number."),
        Horizontal {
            countryCode,
            number,
        },
        _new<AButton>("Next") let {
            connect(it->clicked, this, [this, countryCode, number]() {
                disable();
                auto params = td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>();
                params->phone_number_ = (countryCode->text() + number->text()).toStdString();
                mApp->sendQuery(std::move(params), [this, self = shared_from_this()](App::Object object) {
                    enable();
                });
            });
        },
    }});
    number->focus();
}

void AuthorizationView::handle(td::td_api::authorizationStateWaitCode& state) {
    const char* typeName = "unknown";

    auto code = _new<ATextField>();

    td::td_api::downcast_call(*state.code_info_->type_, aui::lambda_overloaded {
        [&](td::td_api::authenticationCodeTypeTelegramMessage& u) {
            typeName = "Telegram";
            code->setMaxTextLength(u.length_);
        },
        [](auto& u) {

        },
    });
    setContents(Centered { Vertical {
        Label {state.code_info_->phone_number_ } << ".title",
        AText::fromString("A code was sent via {} to your other devices, if you have any connected."_format(typeName)),
        code,

        _new<AButton>("Next") let {
            connect(it->clicked, this, [this, code]() {
                disable();
                mApp->sendQuery(td::td_api::checkAuthenticationCode(code->text().toStdString()), [this, self = shared_from_this()](App::Object object) {
                    enable();
                });
            });
        },
    }});
    code->focus();
}

void AuthorizationView::handle(td::td_api::authorizationStateWaitPassword& state) {
    const char* typeName = "unknown";

    auto password = _new<ATextField>();
    password->setPasswordMode(true);

    setContents(Centered { Vertical {
            Label { "Cloud password check" } << ".title",
            AText::fromString("Please enter your cloud password."),
            password,
            [&]() -> _<ALabel> {
                if (state.password_hint_.empty()) return nullptr;
                return _new<ALabel>("Hint: {}"_format(state.password_hint_));
            }(),
            _new<AButton>("Submit") let {
                connect(it->clicked, this, [this, password]() {
                    disable();
                    mApp->sendQuery(td::td_api::checkAuthenticationPassword(password->text().toStdString()), [this, self = shared_from_this()](App::Object object) {
                        enable();
                    });
                });
            },
    }});
    password->focus();
}
