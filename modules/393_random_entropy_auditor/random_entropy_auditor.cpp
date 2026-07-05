#include "random_entropy_auditor.hpp"

namespace random_entropy_auditor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Random & Entropy Auditor (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
