#include "eye_tracking_analyzer.hpp"

namespace eye_tracking_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Eye-tracking analyzer (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
