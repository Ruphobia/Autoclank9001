#include "hdl_workbench.hpp"

namespace hdl_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HDL workbench (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
