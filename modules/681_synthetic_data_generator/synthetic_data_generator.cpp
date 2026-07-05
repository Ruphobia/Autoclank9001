#include "synthetic_data_generator.hpp"

namespace synthetic_data_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Synthetic data generator (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
