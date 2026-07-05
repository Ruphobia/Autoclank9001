#include "patent_search.hpp"

namespace patent_search {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Patent Search (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
