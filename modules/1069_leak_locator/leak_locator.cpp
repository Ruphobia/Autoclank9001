#include "leak_locator.hpp"

namespace leak_locator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Leak locator (Civil utility (water, waste, sanitation)). Awaits wire-up.";
    return s;
}

}
