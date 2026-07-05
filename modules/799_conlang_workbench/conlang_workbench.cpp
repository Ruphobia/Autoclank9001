#include "conlang_workbench.hpp"

namespace conlang_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Conlang Workbench (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
