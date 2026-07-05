#include "patent_searcher.hpp"

namespace patent_searcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Patent searcher (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
