#include "mail_merge.hpp"

namespace mail_merge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mail merge (Office productivity). Awaits wire-up.";
    return s;
}

}
