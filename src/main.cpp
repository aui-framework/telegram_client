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

#include <AUI/Platform/Entry.h>
#include "App.h"
#include "MyUpdater.h"

#if CLIENT_DEMO
#include "demo.h"
#endif

AUI_ENTRY {
    auto updater = _new<MyUpdater>();
    updater->handleStartup(args);
    auto app = _new<App>(std::move(updater));
    app->run();

#if CLIENT_DEMO
    demo::init(app);
#endif
    return 0;
};