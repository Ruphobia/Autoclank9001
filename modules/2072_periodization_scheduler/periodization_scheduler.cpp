#include "periodization_scheduler.hpp"

namespace periodization_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Periodization scheduler (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
