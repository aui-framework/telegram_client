#pragma once

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>
#include <AUI/Common/AObject.h>
#include <AUI/Common/AMap.h>
#include <AUI/Common/ATimer.h>
#include "MainWindow.h"
#include "model/Chat.h"

class App: public AObject {
public:
    using Object = td::td_api::object_ptr<td::td_api::Object>;

    App();

    void sendQuery(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler = {});

    template<aui::derived_from<td::td_api::Function> F>
    void sendQuery(F f, std::function<void(Object)> handler = {}) {
        sendQuery(td::td_api::make_object<F>(std::move(f)), std::move(handler));
    }

    template<aui::derived_from<td::td_api::Function> F, aui::invocable<typename F::ReturnType::element_type&> Handler>
    void sendQuery(F f, Handler handler) {
        sendQuery(td::td_api::make_object<F>(std::move(f)), [handler = std::move(handler)](Object object) {
            if (object->get_id() == td::td_api::error::ID) {
                return;
            }
            handler(*td::move_tl_object_as<typename F::ReturnType::element_type>(object));
        });
    }

    void run();

    _<App> sharedPtr() {
        return _cast<App>(AObject::sharedPtr());
    }

    const _<Chat>& getChat(int64_t id);

    int64_t myId() const {
        return mMyId;
    }

private:
    _unique<td::ClientManager> mClientManager;
    int64_t mMyId = 0;
    td::ClientManager::ClientId mClientId;
    std::uint64_t mCurrentQueryId{0};
    AMap<std::uint64_t, std::function<void(Object)>> mHandlers;
    AMap<std::int64_t, _<Chat>> mChats;
    _<ATimer> mUpdateTimer;
    _<MainWindow> mWindow;

    void commonHandler(td::tl::unique_ptr<td::td_api::Object> object);
    void processResponse(td::ClientManager::Response response);
    void update();

    void initClientManager();
};
