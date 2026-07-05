#include "authority_record_reconciler.hpp"

namespace authority_record_reconciler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Authority record reconciler (History and archival research). Awaits wire-up.";
    return s;
}

}
