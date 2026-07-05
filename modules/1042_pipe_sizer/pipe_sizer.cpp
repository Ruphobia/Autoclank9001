#include "pipe_sizer.hpp"

namespace pipe_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pipe Sizer (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
