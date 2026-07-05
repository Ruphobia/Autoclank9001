#include "ecg_caliper.hpp"

namespace ecg_caliper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ECG Caliper (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
