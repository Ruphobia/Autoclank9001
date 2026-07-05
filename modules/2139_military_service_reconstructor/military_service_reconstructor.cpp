#include "military_service_reconstructor.hpp"

namespace military_service_reconstructor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Military Service Reconstructor (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
