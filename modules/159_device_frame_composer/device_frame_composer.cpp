#include "device_frame_composer.hpp"

namespace device_frame_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Device frame composer (Mobile development). Awaits wire-up.";
    return s;
}

}
