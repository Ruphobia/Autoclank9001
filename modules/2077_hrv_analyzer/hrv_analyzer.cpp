#include "hrv_analyzer.hpp"

namespace hrv_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HRV analyzer (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
