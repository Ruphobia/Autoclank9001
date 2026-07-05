#include "reference_manager.hpp"

namespace reference_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reference manager (Office productivity). Awaits wire-up.";
    return s;
}

}
