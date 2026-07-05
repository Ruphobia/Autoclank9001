#include "script_designer.hpp"

namespace script_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Script Designer (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
