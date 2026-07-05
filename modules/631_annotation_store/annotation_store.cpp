#include "annotation_store.hpp"

namespace annotation_store {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Annotation store (Documents and reading). Awaits wire-up.";
    return s;
}

}
