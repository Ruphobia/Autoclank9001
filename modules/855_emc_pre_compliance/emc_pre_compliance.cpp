#include "emc_pre_compliance.hpp"

namespace emc_pre_compliance {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: EMC pre-compliance (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
