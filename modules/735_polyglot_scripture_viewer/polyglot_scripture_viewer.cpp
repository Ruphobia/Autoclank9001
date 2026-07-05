#include "polyglot_scripture_viewer.hpp"

namespace polyglot_scripture_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Polyglot Scripture Viewer (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
