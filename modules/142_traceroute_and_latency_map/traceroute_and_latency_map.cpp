#include "traceroute_and_latency_map.hpp"

namespace traceroute_and_latency_map {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Traceroute and latency map (Web development). Awaits wire-up.";
    return s;
}

}
