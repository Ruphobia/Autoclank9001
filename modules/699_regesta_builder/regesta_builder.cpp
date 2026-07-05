#include "regesta_builder.hpp"

namespace regesta_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Regesta builder (History and archival research). Awaits wire-up.";
    return s;
}

}
