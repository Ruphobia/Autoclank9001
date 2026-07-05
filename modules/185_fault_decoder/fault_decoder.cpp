#include "fault_decoder.hpp"

namespace fault_decoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fault decoder (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
