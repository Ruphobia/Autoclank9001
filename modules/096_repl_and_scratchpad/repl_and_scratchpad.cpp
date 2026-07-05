#include "repl_and_scratchpad.hpp"

namespace repl_and_scratchpad {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: REPL and scratchpad (Software development). Awaits wire-up.";
    return s;
}

}
