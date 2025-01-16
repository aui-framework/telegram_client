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


#include <AUI/View/ALabel.h>

class TGIco: public ALabel {
public:
    /**
     * @brief Open assets/tgico.ttf in font editor like fontforge
     */
    enum Icon {
        NONE = 0,
        UNREAD = 0xe900,
        READ,
        MOBILE_DESKTOP,
        PLUS,
        HEART_PLUS,
        ACTION_PLUS,
        PERSON_PLUS1,
        PERSON_PLUS2,
        ADMIN,
        POLL,
        CAT,
        SPUTNIK,
        EXPAND,
        SHRINK,
        DOWNLOADS,
        ARROW_DOWN,
        ARROW_RIGHT,
        ARROW_LEFT,
        ARROW_DOWN_CIRCLED,
        DOWN,
        PAPER_CLIP,
        REPEAT,
        REPEAT1,
        INCOGNITO,
        RIGHT,
        LEFT,
        TRASH,
        BOLD,
        BOMB,
        ACTION,
        ACTION_CIRCLED,
        MAC_CMD,
        ANDROID,
        XYI_ZNAET,
        DEBUG,
        CALENDAR_BIG,
        CANLENDAR,
        CAMERA,
        CAMERA_PLUS,
        ALIGN_BOTTOM,
        ALIGN_TOP,
        CAR,
        CREDIT_CARD1,
        CREDIT_CARD2,
        DOLLAR,
        CHANNEL,
        EYE1,
        PINNED,
        DISCUSSIONS,
        CHECKMARK1,
        CHECKMARK2,
        CHECKMARK3_STRIPED,
        CHECKMARK2_EMPTY,
        CHECKMARK3,
        CHECKMARK2_REPEAT,
        CHECKMARK4,
        CHECKMARK5,
        CHECKMARK5_CLOCK,
        CLOSE,
        DOWNLOAD_FROM_CLOUD,
        SHRINK2,
        PIPETTE,
        DISCUSSIONS2,
        DISCUSSIONS3,
        COPY,
        NO_GIF,
        CIRCLED_X,
        NO_STAR,
        SLEEP,
        DATABASE,
        TRASH2,
        TRASH3,
        GHOST,
        BACKSPACE,
        NO_PERSON,
        EYE2,
        EYE2_STRIPED,
        EYE1_STRIPED,
        VERIFIED,
        SEEK_FORWARD,
        SEEK_BACKWARD,
        STAR,
        FLAG,
        SYNC,
        FOLDER,
        TRANSLATE,
        REPLY1,
        REPLY2,

    };
    using ALabel::ALabel;
    ~TGIco() override = default;

    void setIconHideIfNone(TGIco::Icon icon) {
        if (icon == NONE) {
            setVisibility(Visibility::GONE);
            return;
        }
        setText({ char16_t(icon) });
    }
};

template<>
struct ADataBindingDefault<TGIco, AString> {
public:
    static auto property(const _<ALabel>& view) {
        return view->text();
    }

    static void setup(const _<ALabel>& view) {}
};
