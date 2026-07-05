#include "ham_antenna_designer.hpp"

namespace ham_antenna_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ham antenna designer (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
