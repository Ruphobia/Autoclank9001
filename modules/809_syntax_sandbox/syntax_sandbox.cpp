#include "syntax_sandbox.hpp"

namespace syntax_sandbox {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Syntax Sandbox (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
