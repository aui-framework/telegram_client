// AUI Telegram Client - example of using AUI Framework
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

//
// Created by alex2772 on 12/21/24.
//

#include <AUI/Util/AImageDrawable.h>
#include "demo.h"
#include "view/MainView.h"

using namespace std::chrono_literals;

void demo::init(const _<App>& app) {
    auto main = _new<MainView>(app);

    main->chatListView()->setModel(AListModel<_<Chat>>::fromVector(AVector<_<Chat>> {
      _new<Chat>(ChatModel {
        .id = 1,
        .title = "Anti Kira Forces",
        .previewText = "soichiro: Sticker",
        .lastMessage = nullptr,
        .time = "8:58 PM",
        .thumbnail = IDrawable::fromUrl("6.jpg"),
        .inboxLastReadMessage = 0,
        .outboxLastReadMessage = 0,
        .messages = AListModel<_<Message>>::fromVector(AVector<_<Message>> {
          _new<Message>(MessageModel {
            .userId = 3,
            .date = std::chrono::system_clock::from_time_t(532352),
            .content =
                MessageModel::Content {
                  .text = "I am currently in a room at the Teito Hotel",
                  .photo =
                      MessageModel::
                          Content::
                              Photo {
                                .drawable = _new<AImageDrawable>(AImage::fromUrl("8.jpg")),
                                .size = { 300, 200 },
                              },
                },
          }),
          _new<Message>(MessageModel {
            .userId = 3,
            .date = std::chrono::system_clock::from_time_t(532352 + 50),
            .content =
                MessageModel::Content {
                  .text = "I shall be switching hotels from now on every few days",
                },
          }),
          _new<Message>(MessageModel {
            .userId = 3,
            .date = std::chrono::system_clock::from_time_t(532352 + 50 * 2),
            .content =
                MessageModel::Content {
                  .text = "From now on, I want you to think of these rooms as the de facto HQ of the investigation",
                },
          }),
          _new<Message>(MessageModel {
            .userId = 3,
            .date = std::chrono::system_clock::from_time_t(532352 + 50 * 3),
            .content =
                MessageModel::Content {
                  .text = "If you can agree to these terms I want you to split into two 2 groups, with a 30-minute "
                          "interval between you, and come here by twelve o'clock midnight",
                },
          }),
          _new<Message>(MessageModel {
            .content =
                MessageModel::Content {
                  .text = "OK!",
                },
          }),
        }) }),
      _new<Chat>(ChatModel {
        .id = 1,
        .title = "Pavel",
        .previewText = "This one looks better",
        .lastMessage = nullptr,
        .time = "7:13 PM",
        .thumbnail = IDrawable::fromUrl("1.jpg"),
        .inboxLastReadMessage = 1,
        .outboxLastReadMessage = 1,
      }),
      _new<Chat>(ChatModel {
        .id = 1,
        .title = "Butcher",
        .previewText = "Look who's coming for dinner!",
        .lastMessage = nullptr,
        .time = "6:28 PM",
        .thumbnail = IDrawable::fromUrl("2.jpg"),
        .inboxLastReadMessage = 1,
        .outboxLastReadMessage = 1,
      }),
      _new<Chat>(ChatModel {
        .id = 1,
        .title = "Wild Cats",
        .previewText = "Photo",
        .lastMessage = nullptr,
        .time = "1:07 PM",
        .thumbnail = IDrawable::fromUrl("3.jpg"),
        .inboxLastReadMessage = 1,
        .outboxLastReadMessage = 1,
        .unreadCount = 3,
      }),
      _new<Chat>(ChatModel {
        .id = 1,
        .title = "Vi",
        .previewText = "doesn't matter how good you are in night city",
        .lastMessage = nullptr,
        .lastSendStatus = MessageModel::SendStatus::READ,
        .time = "7:00 AM",
        .thumbnail = IDrawable::fromUrl("4.jpg"),
        .inboxLastReadMessage = 0,
        .outboxLastReadMessage = 0,
      }),
      _new<Chat>(ChatModel {
        .id = 1,
        .title = "Yen <3",
        .previewText = "let's go stare at the sky like in the sh*tty two crown romance",
        .lastMessage = nullptr,
        .time = "Fri",
        .thumbnail = IDrawable::fromUrl("5.jpg"),
        .inboxLastReadMessage = 0,
        .outboxLastReadMessage = 0,
      }),
      _new<Chat>(ChatModel {
        .id = 1,
        .title = "P-3",
        .previewText = "You're telling me that jellyman can sneak up behind someone and kill them instantly without "
                       "leaving a trace?",
        .lastMessage = nullptr,
        .time = "Mon",
        .thumbnail = IDrawable::fromUrl("7.jpg"),
        .inboxLastReadMessage = 0,
        .outboxLastReadMessage = 0,
      }),
    }));

    app->window()->present(std::move(main));
}