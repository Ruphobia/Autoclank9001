#include "swo_itm_rtt_console.hpp"

namespace swo_itm_rtt_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SWO/ITM/RTT console (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
