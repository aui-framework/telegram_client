#include "MainWindow.h"
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/ALabel.h>
#include <AUI/View/AButton.h>
#include <AUI/Platform/APlatform.h>
#include <AUI/View/ADrawableView.h>
#include <AUI/Util/ALayoutInflater.h>

using namespace declarative;

MainWindow::MainWindow(_<App> app): AWindow("Example telegram client", 800_dp, 500_dp), mApp(std::move(app)) {
    setContents(
        Centered{
            Label { "Starting..." }
        }
    );
}

void MainWindow::present(_<AView> view) {
    ALayoutInflater::inflate(this, std::move(view));
}
