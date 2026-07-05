#include "weibull_reliability_analyzer.hpp"

namespace weibull_reliability_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Weibull reliability analyzer (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
