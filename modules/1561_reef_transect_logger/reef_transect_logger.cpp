#include "reef_transect_logger.hpp"

namespace reef_transect_logger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reef transect logger (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
