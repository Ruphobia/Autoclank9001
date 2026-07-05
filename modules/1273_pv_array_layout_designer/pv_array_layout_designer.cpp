#include "pv_array_layout_designer.hpp"

namespace pv_array_layout_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PV Array Layout Designer (Energy systems). Awaits wire-up.";
    return s;
}

}
