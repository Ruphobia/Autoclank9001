#include "translation_workbench.hpp"

namespace translation_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Translation Workbench (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
