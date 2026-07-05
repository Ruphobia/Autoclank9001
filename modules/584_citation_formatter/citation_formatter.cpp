#include "citation_formatter.hpp"

namespace citation_formatter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Citation Formatter (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
