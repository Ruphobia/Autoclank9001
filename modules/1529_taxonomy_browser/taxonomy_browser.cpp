#include "taxonomy_browser.hpp"

namespace taxonomy_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Taxonomy browser (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
