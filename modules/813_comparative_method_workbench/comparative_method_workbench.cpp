#include "comparative_method_workbench.hpp"

namespace comparative_method_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Comparative Method Workbench (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
