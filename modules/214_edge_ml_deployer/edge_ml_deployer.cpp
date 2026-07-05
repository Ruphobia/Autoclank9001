#include "edge_ml_deployer.hpp"

namespace edge_ml_deployer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Edge ML deployer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
