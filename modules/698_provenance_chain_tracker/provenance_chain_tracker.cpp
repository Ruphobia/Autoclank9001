#include "provenance_chain_tracker.hpp"

namespace provenance_chain_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Provenance chain tracker (History and archival research). Awaits wire-up.";
    return s;
}

}
