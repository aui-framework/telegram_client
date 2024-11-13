#pragma once


#include <AUI/View/AViewContainer.h>
#include "App.h"

class MainView: public AViewContainer {
public:
    MainView(_<App> app);

    void logout();

private:
    _<App> mApp;
    _<AViewContainer> mChatWrap;

    void inflateChatPlaceholder();

    void presentChat(_<Chat> chat);
};
