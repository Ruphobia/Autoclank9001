#include "flow_cytometry_workbench.hpp"

namespace flow_cytometry_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Flow cytometry workbench (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
