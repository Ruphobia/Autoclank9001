#include "incompatibility_checker.hpp"

namespace incompatibility_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Incompatibility checker (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
