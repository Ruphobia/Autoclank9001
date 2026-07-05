#include "critical_apparatus_builder.hpp"

namespace critical_apparatus_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Critical apparatus builder (History and archival research). Awaits wire-up.";
    return s;
}

}
