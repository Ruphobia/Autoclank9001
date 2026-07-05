#include "dataset_curator.hpp"

namespace dataset_curator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dataset curator (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
