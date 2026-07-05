#include "rigging_load_calculator.hpp"

namespace rigging_load_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rigging Load Calculator (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
