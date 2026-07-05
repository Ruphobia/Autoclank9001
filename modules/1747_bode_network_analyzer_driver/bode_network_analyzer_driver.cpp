#include "bode_network_analyzer_driver.hpp"

namespace bode_network_analyzer_driver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bode/network analyzer driver (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
