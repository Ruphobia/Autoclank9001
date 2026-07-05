#include "meeting_minutes_extractor.hpp"

namespace meeting_minutes_extractor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Meeting minutes extractor (Office productivity). Awaits wire-up.";
    return s;
}

}
