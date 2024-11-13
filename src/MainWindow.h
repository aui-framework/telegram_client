#pragma once

#include <AUI/Platform/AWindow.h>

class App;

class MainWindow: public AWindow {
public:
    MainWindow(_<App> app);

    void present(_<AView> view);

private:
    _<App> mApp;
};
