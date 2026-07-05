#include "activation_explorer.hpp"

namespace activation_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Activation explorer (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
