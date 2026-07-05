#include "frame_analyzer.hpp"

namespace frame_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Frame Analyzer (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
