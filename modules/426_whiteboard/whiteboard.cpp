#include "whiteboard.hpp"

namespace whiteboard {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Whiteboard (Office productivity). Awaits wire-up.";
    return s;
}

}
