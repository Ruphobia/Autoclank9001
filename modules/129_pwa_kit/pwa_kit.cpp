#include "pwa_kit.hpp"

namespace pwa_kit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PWA kit (Web development). Awaits wire-up.";
    return s;
}

}
