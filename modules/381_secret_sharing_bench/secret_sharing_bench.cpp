#include "secret_sharing_bench.hpp"

namespace secret_sharing_bench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Secret Sharing Bench (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
