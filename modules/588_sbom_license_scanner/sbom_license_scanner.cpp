#include "sbom_license_scanner.hpp"

namespace sbom_license_scanner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SBOM License Scanner (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
