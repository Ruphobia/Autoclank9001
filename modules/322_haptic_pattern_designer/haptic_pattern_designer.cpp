#include "haptic_pattern_designer.hpp"

namespace haptic_pattern_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Haptic Pattern Designer (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
