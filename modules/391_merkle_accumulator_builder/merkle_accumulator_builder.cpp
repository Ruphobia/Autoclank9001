#include "merkle_accumulator_builder.hpp"

namespace merkle_accumulator_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Merkle & Accumulator Builder (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
