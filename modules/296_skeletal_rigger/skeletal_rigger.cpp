#include "skeletal_rigger.hpp"

namespace skeletal_rigger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Skeletal Rigger (Game development). Awaits wire-up.";
    return s;
}

}
