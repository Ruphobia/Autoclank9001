#include "code_checker.hpp"

namespace code_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Code Checker (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
