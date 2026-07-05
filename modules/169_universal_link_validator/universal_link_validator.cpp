#include "universal_link_validator.hpp"

namespace universal_link_validator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Universal Link validator (Mobile development). Awaits wire-up.";
    return s;
}

}
