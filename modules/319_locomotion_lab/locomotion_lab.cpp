#include "locomotion_lab.hpp"

namespace locomotion_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Locomotion Lab (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
