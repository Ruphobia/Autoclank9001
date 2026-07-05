#include "pathway_browser.hpp"

namespace pathway_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pathway Browser (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
