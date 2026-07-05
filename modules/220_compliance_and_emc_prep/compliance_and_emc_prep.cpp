#include "compliance_and_emc_prep.hpp"

namespace compliance_and_emc_prep {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Compliance and EMC prep (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
