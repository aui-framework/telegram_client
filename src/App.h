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

#pragma once

#include <Telegram.h>
#include <AUI/Common/AObject.h>
#include <AUI/Common/AMap.h>
#include <AUI/Common/ATimer.h>
#include "MainWindow.h"
#include "MyUpdater.h"
#include "model/ChatList.h"
#include "Photo.h"

struct Chat;

class App: public AObject {
public:
    using Object = td::td_api::object_ptr<td::td_api::Object>;

    App(_<MyUpdater> updater);

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

    template<aui::derived_from<td::td_api::Function> F, aui::invocable<typename F::ReturnType::element_type&> Handler>
    void sendQuery(td::td_api::object_ptr<F> f, Handler handler) {
        sendQuery(std::move(f), [handler = std::move(handler)](Object object) {
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
    const _<MainWindow>& window() const { return mWindow; }
    const _<MyUpdater>& autoUpdater() const { return mAutoUpdater; }

    AProperty<bool> hasPendingNetworkActivity = true;
    _<ChatList> getChatList(ChatList::Kind kind);

    photo::Cache& photos() {
        return mPhotoCache;
    }

private:
    _unique<td::ClientManager> mClientManager;
    int64_t mMyId = 0;
    size_t mQueryCountLastUpdate = 0;
    bool mWarmupComplete = false;
    td::ClientManager::ClientId mClientId;
    std::uint64_t mCurrentQueryId{0};
    AMap<std::uint64_t, std::function<void(Object)>> mHandlers;
    AMap<std::int64_t, _<Chat>> mChats;
    AUnorderedMap<ChatList::Kind, _<ChatList>> mChatLists;
    _<ATimer> mTgUpdateTimer;
    _<ATimer> mAutoUpdateTimer;
    _<MainWindow> mWindow;
    _<MyUpdater> mAutoUpdater;
    photo::Cache mPhotoCache;

    void commonHandler(td::tl::unique_ptr<td::td_api::Object> object);
    void processResponse(td::ClientManager::Response response);
    void update();

    void initClientManager();
};
