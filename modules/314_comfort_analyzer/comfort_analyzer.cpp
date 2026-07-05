#include "comfort_analyzer.hpp"

namespace comfort_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Comfort Analyzer (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
