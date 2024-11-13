#pragma once


#include <AUI/View/AViewContainer.h>
#include <AUI/View/ATextField.h>
#include "App.h"

class ChatView: public AViewContainer {
public:
    ChatView(_<App> app, _<Chat> chat);

private:
    _<App> mApp;
    _<Chat> mChat;
    _<ATextField> mInput;
    _<AViewContainer> mContentsWrap;

    void send();
};
