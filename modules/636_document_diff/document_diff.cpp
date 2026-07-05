#include "document_diff.hpp"

namespace document_diff {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Document diff (Documents and reading). Awaits wire-up.";
    return s;
}

}
