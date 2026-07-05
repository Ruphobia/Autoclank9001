#include "inter_rater_reliability_calculator.hpp"

namespace inter_rater_reliability_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Inter-rater reliability calculator (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
