#include "case_law_search.hpp"

namespace case_law_search {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Case Law Search (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
