#include "document_diff_and_merge.hpp"

namespace document_diff_and_merge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Document diff and merge (Office productivity). Awaits wire-up.";
    return s;
}

}
