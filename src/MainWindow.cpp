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

#include "MainWindow.h"

#include <AUI/Platform/APlatform.h>
#include <AUI/Util/ALayoutInflater.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AButton.h>
#include <AUI/View/ADrawableView.h>
#include <AUI/View/ALabel.h>
#include <AUI/View/AScrollbar.h>
#include <AUI/Util/ANoiseDrawable.h>
#include <AUI/View/AText.h>
#include <AUI/View/ASpinner.h>

#include "view/TGIco.h"
#include <App.h>
#include <AUI/View/ASpinnerV2.h>

using namespace declarative;

MainWindow::MainWindow(_<App> app)
  : ACustomCaptionWindow("AUIgram", 800_dp, 500_dp, true), mApp(std::move(app)) {
    getContentContainer()->setContents(Centered { Label { "Starting..." } });
    *this << ".container_chat_background_color";
    setExtraStylesheet({
      {
        t<AWindowBase>(),
        Padding { 1_px },
        MinSize { 400_dp, 200_dp },
      },
      {
        c(".window-title"),
        BackgroundSolid { nullptr },
      },
      {
        t<ASpinnerV2>(),
        BackgroundImage { {}, 0x50ffffff_argb },
      }
    });

    getContentContainer() << ".container_chat_background_color";
    getCaptionContainer()->setContents(Stacked {
      Horizontal::Expanding {
        _new<ASpacerExpanding>(),
        Centered {
          _new<ASpinnerV2>() & mApp->hasPendingNetworkActivity > &AView::setVisible,
        },
      },
#if !CLIENT_DEMO
      Centered {
          Label { "Proof of concept. Not a production app" } with_style { Opacity(0.5f), TextColor { AColor::RED } },
      },
#endif
    });
    getContentContainer()->setExtraStylesheet({
      // clear aui default styles.
      {
        { t<ALabel>(), t<AText>(), c(".send_input") },
        TextColor { 0xf3f3f3_rgb },
        BackgroundSolid { nullptr },
        Border { nullptr },
        BoxShadow { nullptr },
        FontRendering::ANTIALIASING,
      },
      {
        t<TGIco>(),
        FontRendering::ANTIALIASING,
        Font { ":tgico.ttf" },
        Padding { 0 },
        Margin { 0 },
      },


      {
        c(".container_color"),
        BackgroundSolid { 0x16202a_rgb },
      },
      {
        c(".container_color_blur"),
        Backdrop { Backdrop::GaussianBlur { 64_dp } },
        BackgroundSolid { (0x16202a_rgb).transparentize(0.3f) },
        BackgroundImage { _new<ANoiseDrawable>(), 0x02ffffff_argb },
      },
      {
        c(".message"),
        BackgroundSolid { 0x172433_rgb },
        BorderRadius { 13_dp },
        Margin { 4_dp },
      },
      {
        c(".message_mine"),
        BackgroundSolid { 0x2a5277_rgb },
      },
      {
        c(".message_has_photo"),
        AOverflow::HIDDEN,
      },
      {
        c(".accent_textcolor"),
        TextColor { 0x589fd7_rgb },
      },
      {
        c(".container_chat_background_color"),
        BackgroundSolid { 0x0d1520_rgb },
      },

      {
        t<AScrollbarHandle>(),
        BackgroundSolid { 0x4f555b_rgb },
      },
      {
        c(".unread_count"),
        BackgroundSolid { 0x4082bc_rgb },
      },
    });
}

void MainWindow::present(_<AView> view) {
    ALayoutInflater::inflate(getContentContainer(), std::move(view));
}
