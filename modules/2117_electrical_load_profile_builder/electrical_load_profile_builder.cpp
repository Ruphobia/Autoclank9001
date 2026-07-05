#include "electrical_load_profile_builder.hpp"

namespace electrical_load_profile_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Electrical load profile builder (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
