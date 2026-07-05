#include "sound_transmission_loss_calculator.hpp"

namespace sound_transmission_loss_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sound transmission loss calculator (Acoustic, thermal, hydraulic, pneumatic engineering). Awaits wire-up.";
    return s;
}

}
