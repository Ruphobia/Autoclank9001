#include "mortgage_scenario_comparator.hpp"

namespace mortgage_scenario_comparator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mortgage scenario comparator (Real estate, insurance, actuarial). Awaits wire-up.";
    return s;
}

}
