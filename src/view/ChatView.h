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


#include <AUI/View/AViewContainer.h>
#include <AUI/View/ATextArea.h>
#include "App.h"
#include <model/Message.h>

class ChatView: public AViewContainer {
public:
    ChatView(_<App> app, _<Chat> chat);
    ~ChatView() override;

private:
    _<App> mApp;
    _<Chat> mChat;
    _<AScrollArea> mScrollArea;
    _<ATextArea> mInput;
    _<AViewContainer> mContentsWrap;
    AVector<int64_t /* message id */> mReadMessagesBatch;

    template<typename MessageModelT = Message>
    _<AViewContainer> makeMessage(const _<MessageModelT>& message);
    void send();
};
