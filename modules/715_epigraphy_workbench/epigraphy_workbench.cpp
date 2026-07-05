#include "epigraphy_workbench.hpp"

namespace epigraphy_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Epigraphy Workbench (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
