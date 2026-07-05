#include "winetastingnotes.hpp"

namespace winetastingnotes {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: WineTastingNotes (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
