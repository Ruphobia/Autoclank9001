#include "ppe_selector.hpp"

namespace ppe_selector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PPE selector (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
