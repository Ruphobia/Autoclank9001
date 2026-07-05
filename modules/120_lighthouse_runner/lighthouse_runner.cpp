#include "lighthouse_runner.hpp"

namespace lighthouse_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lighthouse runner (Web development). Awaits wire-up.";
    return s;
}

}
