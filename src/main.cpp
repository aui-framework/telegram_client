#include <AUI/Platform/Entry.h>
#include "App.h"

AUI_ENTRY {
    _new<App>()->run();
    return 0;
};