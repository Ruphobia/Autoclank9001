#include "layout_inspector.hpp"

namespace layout_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Layout inspector (Web development). Awaits wire-up.";
    return s;
}

}
