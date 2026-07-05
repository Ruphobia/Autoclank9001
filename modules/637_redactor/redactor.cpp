#include "redactor.hpp"

namespace redactor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Redactor (Documents and reading). Awaits wire-up.";
    return s;
}

}
