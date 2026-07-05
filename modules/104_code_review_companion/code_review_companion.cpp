#include "code_review_companion.hpp"

namespace code_review_companion {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Code review companion (Software development). Awaits wire-up.";
    return s;
}

}
