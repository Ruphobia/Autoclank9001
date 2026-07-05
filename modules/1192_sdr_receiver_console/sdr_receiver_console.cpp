#include "sdr_receiver_console.hpp"

namespace sdr_receiver_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SDR receiver console (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
