#include "fallacy_detector.hpp"

namespace fallacy_detector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fallacy detector (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
