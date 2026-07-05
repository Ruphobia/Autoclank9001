#include "weldment_designer.hpp"

namespace weldment_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Weldment designer (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
