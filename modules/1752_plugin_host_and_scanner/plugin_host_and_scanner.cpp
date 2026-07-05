#include "plugin_host_and_scanner.hpp"

namespace plugin_host_and_scanner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Plugin host and scanner (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
