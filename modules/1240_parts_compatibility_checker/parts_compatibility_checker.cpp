#include "parts_compatibility_checker.hpp"

namespace parts_compatibility_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Parts Compatibility Checker (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
