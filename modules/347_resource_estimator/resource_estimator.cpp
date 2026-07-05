#include "resource_estimator.hpp"

namespace resource_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Resource Estimator (Quantum computing). Awaits wire-up.";
    return s;
}

}
