#include "zk_circuit_playground.hpp"

namespace zk_circuit_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ZK Circuit Playground (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
