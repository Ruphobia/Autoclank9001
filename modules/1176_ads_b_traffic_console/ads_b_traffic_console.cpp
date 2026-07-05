#include "ads_b_traffic_console.hpp"

namespace ads_b_traffic_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ADS-B traffic console (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
