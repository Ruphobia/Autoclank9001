#include "probing_and_metrology_suite.hpp"

namespace probing_and_metrology_suite {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Probing and metrology suite (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
