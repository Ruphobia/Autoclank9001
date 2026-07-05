#include "composition_overlay.hpp"

namespace composition_overlay {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Composition Overlay (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
