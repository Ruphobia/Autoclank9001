#include "training_load_pmc.hpp"

namespace training_load_pmc {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Training load PMC (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
