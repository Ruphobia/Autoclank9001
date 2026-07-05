#include "digital_mode_bridge.hpp"

namespace digital_mode_bridge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Digital mode bridge (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
