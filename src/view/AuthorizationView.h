#pragma once


#include <AUI/View/AViewContainer.h>
#include "App.h"

class AuthorizationView: public AViewContainer {
public:
    AuthorizationView(_<App> app);

    void handle(td::td_api::authorizationStateWaitPhoneNumber& state);
    void handle(td::td_api::authorizationStateWaitCode& state);
    void handle(td::td_api::authorizationStateWaitPassword& state);

private:
    _<App> mApp;


};
