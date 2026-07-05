#include "condition_and_conservation_log.hpp"

namespace condition_and_conservation_log {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Condition and Conservation Log (Numismatics, philately, collecting, antiques). Awaits wire-up.";
    return s;
}

}
