#include "historical_patent_browser.hpp"

namespace historical_patent_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Historical patent browser (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
