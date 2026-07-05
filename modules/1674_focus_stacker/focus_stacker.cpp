#include "focus_stacker.hpp"

namespace focus_stacker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Focus Stacker (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
