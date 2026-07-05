#include "task_inbox.hpp"

namespace task_inbox {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Task inbox (Office productivity). Awaits wire-up.";
    return s;
}

}
