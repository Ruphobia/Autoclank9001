#include "cap_rate_and_noi_calculator.hpp"

namespace cap_rate_and_noi_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cap rate and NOI calculator (Real estate, insurance, actuarial). Awaits wire-up.";
    return s;
}

}
