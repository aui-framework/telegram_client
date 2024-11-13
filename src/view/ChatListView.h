#pragma once


#include <AUI/View/AViewContainer.h>
#include "App.h"

class ChatListView: public AViewContainer {
public:
    ChatListView(_<App> chat);

signals:
    emits<_<Chat>> chatSelected;

private:
    _<App> mApp;
};
