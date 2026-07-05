#include "trap_line_manager.hpp"

namespace trap_line_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Trap Line Manager (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
