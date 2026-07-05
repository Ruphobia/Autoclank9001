#include "behavioral_event_coder.hpp"

namespace behavioral_event_coder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Behavioral event coder (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
