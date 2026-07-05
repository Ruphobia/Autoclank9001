#include "citation_and_footnote_formatter.hpp"

namespace citation_and_footnote_formatter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Citation and footnote formatter (History and archival research). Awaits wire-up.";
    return s;
}

}
