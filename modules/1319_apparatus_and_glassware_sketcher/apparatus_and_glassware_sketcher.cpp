#include "apparatus_and_glassware_sketcher.hpp"

namespace apparatus_and_glassware_sketcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Apparatus and glassware sketcher (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
