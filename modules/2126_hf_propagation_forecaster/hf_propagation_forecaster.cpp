#include "hf_propagation_forecaster.hpp"

namespace hf_propagation_forecaster {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HF propagation forecaster (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
