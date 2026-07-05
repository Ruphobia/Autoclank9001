#include "anatomy_atlas_viewer.hpp"

namespace anatomy_atlas_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Anatomy Atlas Viewer (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
