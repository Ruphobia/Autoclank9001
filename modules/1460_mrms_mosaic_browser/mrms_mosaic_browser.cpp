#include "mrms_mosaic_browser.hpp"

namespace mrms_mosaic_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MRMS mosaic browser (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
