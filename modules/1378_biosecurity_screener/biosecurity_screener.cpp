#include "biosecurity_screener.hpp"

namespace biosecurity_screener {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Biosecurity screener (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
