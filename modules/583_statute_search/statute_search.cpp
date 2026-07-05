#include "statute_search.hpp"

namespace statute_search {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Statute Search (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
