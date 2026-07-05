#include "feature_store.hpp"

namespace feature_store {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Feature store (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
