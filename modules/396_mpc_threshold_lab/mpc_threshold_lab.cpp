#include "mpc_threshold_lab.hpp"

namespace mpc_threshold_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MPC Threshold Lab (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
