#include "stabilizer.hpp"

namespace stabilizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stabilizer (Image and video). Awaits wire-up.";
    return s;
}

}
