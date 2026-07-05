#include "timeline_composer.hpp"

namespace timeline_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Timeline composer (History and archival research). Awaits wire-up.";
    return s;
}

}
