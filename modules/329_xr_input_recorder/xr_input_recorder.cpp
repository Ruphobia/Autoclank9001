#include "xr_input_recorder.hpp"

namespace xr_input_recorder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: XR Input Recorder (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
