#include "bench_instrument_bridge.hpp"

namespace bench_instrument_bridge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bench instrument bridge (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
