#include "tilemap_composer.hpp"

namespace tilemap_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tilemap Composer (Game development). Awaits wire-up.";
    return s;
}

}
