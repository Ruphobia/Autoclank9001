#include "lock_and_deadlock_inspector.hpp"

namespace lock_and_deadlock_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lock and deadlock inspector (Databases). Awaits wire-up.";
    return s;
}

}
