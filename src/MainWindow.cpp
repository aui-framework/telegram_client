#include "MainWindow.h"
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/ALabel.h>
#include <AUI/View/AButton.h>
#include <AUI/Platform/APlatform.h>
#include <AUI/View/ADrawableView.h>

using namespace declarative;

MainWindow::MainWindow(_<MyUpdater> updater):
    AWindow("Project template app", 300_dp, 200_dp),
    mUpdater(std::move(updater)) {
    setContents(Centered { Vertical {
      Centered { Icon { ":img/icon.svg" } with_style { FixedSize(64_dp) } },
      Centered { Label { "Hello world from AUI!" } },
      _new<AButton>("Visit GitHub repo")
          .connect(&AView::clicked, this, [] { APlatform::openUrl("https://github.com/aui-framework/aui"); }),
      _new<AButton>("Visit docs")
          .connect(&AView::clicked, this, [] { APlatform::openUrl("https://aui-framework.github.io/"); }),
      _new<AButton>("Submit an issue")
          .connect(
              &AView::clicked, this, [] { APlatform::openUrl("https://github.com/aui-framework/aui/issues/new"); }),
      CustomLayout {} & mUpdater->status.readProjected([&updater = mUpdater](const AUpdater::Status& status) {
        return std::visit(aui::lambda_overloaded {
          [&](const AUpdater::StatusIdle&) -> _<AView> {
              return _new<AButton>("Check for updates").connect(&AView::clicked, slot(updater)::checkForUpdates);
          },
          [&](const AUpdater::StatusCheckingForUpdates&) -> _<AView>  {
            return Label { "Checking for updates..." };
          },
          [&](const AUpdater::StatusDownloading&) -> _<AView>  {
            return Label { "Downloading..." };
          },
          [&](const AUpdater::StatusWaitingForApplyAndRestart&) -> _<AView>  {
            return _new<AButton>("Apply update and restart").connect(&AView::clicked, slot(updater)::applyUpdateAndRestart);
          },
        }, status);
      }),
      Label { "Btw, 2 + 2 = {}"_format(sum(2, 2)) },
      Label { "Version: " AUI_PP_STRINGIZE(AUI_CMAKE_PROJECT_VERSION) },
    } });
}

int MainWindow::sum(int a, int b) { return a + b; }
