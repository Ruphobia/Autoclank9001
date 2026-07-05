#include "annotation_workbench.hpp"

namespace annotation_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Annotation Workbench (Memory and knowledge accrual). Awaits wire-up.";
    return s;
}

}
