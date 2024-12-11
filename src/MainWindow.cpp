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

#include "MainWindow.h"

#include <AUI/Platform/APlatform.h>
#include <AUI/Util/ALayoutInflater.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AButton.h>
#include <AUI/View/ADrawableView.h>
#include <AUI/View/ALabel.h>
#include <AUI/View/AScrollbar.h>

#include "view/TGIco.h"

using namespace declarative;

MainWindow::MainWindow(_<App> app) : ACustomCaptionWindow("AUI Telegram Client", 800_dp, 500_dp), mApp(std::move(app)) {
    getContentContainer()->setContents(Centered { Label { "Starting..." } });
    setExtraStylesheet({
      {
        t<AWindowBase>(),
        BackgroundSolid { 0x0d1520_rgb },
        Padding { 1_px },
        MinSize { 400_dp, 300_dp },
      },
      {
        c(".window-title"),
        BackgroundSolid { nullptr },
      },
    });

    getCaptionContainer()->setContents(Centered {
      Label { "Proof of concept. Not a production app" } with_style { Opacity(0.5f), TextColor { AColor::RED } },
    });
    getContentContainer()->setExtraStylesheet({
      // clear aui default styles.
      {
        t<AView>(),
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
        c(".message") >> c(".status"),
        TextColor { 0x589fd7_rgb },
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

void MainWindow::present(_<AView> view) { ALayoutInflater::inflate(getContentContainer(), std::move(view)); }
