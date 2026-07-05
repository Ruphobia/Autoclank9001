#include "lucid_technique_tracker.hpp"

namespace lucid_technique_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lucid Technique Tracker (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
