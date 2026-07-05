#include "palette_workshop.hpp"

namespace palette_workshop {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Palette Workshop (Game development). Awaits wire-up.";
    return s;
}

}
