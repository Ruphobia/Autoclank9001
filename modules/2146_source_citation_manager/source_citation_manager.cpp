#include "source_citation_manager.hpp"

namespace source_citation_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Source Citation Manager (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
