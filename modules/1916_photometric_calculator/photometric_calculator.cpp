#include "photometric_calculator.hpp"

namespace photometric_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Photometric Calculator (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
