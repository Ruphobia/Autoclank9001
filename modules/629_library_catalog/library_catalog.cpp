#include "library_catalog.hpp"

namespace library_catalog {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Library catalog (Documents and reading). Awaits wire-up.";
    return s;
}

}
