#include "liveness_runner.hpp"

namespace liveness_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Liveness Runner (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
