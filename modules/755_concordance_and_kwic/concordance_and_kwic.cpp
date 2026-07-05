#include "concordance_and_kwic.hpp"

namespace concordance_and_kwic {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Concordance and KWIC (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
