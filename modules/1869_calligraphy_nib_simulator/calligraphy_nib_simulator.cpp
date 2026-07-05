#include "calligraphy_nib_simulator.hpp"

namespace calligraphy_nib_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Calligraphy nib simulator (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
