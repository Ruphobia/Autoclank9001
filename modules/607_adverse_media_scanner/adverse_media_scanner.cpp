#include "adverse_media_scanner.hpp"

namespace adverse_media_scanner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Adverse Media Scanner (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
