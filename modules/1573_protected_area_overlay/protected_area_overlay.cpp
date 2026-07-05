#include "protected_area_overlay.hpp"

namespace protected_area_overlay {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Protected area overlay (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
