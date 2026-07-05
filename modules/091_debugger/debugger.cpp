#include "debugger.hpp"

namespace debugger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Debugger (Software development). Awaits wire-up.";
    return s;
}

}
