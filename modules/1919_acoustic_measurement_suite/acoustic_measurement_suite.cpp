#include "acoustic_measurement_suite.hpp"

namespace acoustic_measurement_suite {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Acoustic Measurement Suite (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
