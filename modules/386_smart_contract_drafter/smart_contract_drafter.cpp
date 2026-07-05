#include "smart_contract_drafter.hpp"

namespace smart_contract_drafter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Smart Contract Drafter (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
