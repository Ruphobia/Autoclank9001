#include "lab_protocol_and_eln.hpp"

namespace lab_protocol_and_eln {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lab protocol and ELN (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
