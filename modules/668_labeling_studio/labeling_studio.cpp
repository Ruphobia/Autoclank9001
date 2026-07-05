#include "labeling_studio.hpp"

namespace labeling_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Labeling studio (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
