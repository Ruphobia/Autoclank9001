#include "frequency_severity_simulator.hpp"

namespace frequency_severity_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Frequency-severity simulator (Real estate, insurance, actuarial). Awaits wire-up.";
    return s;
}

}
