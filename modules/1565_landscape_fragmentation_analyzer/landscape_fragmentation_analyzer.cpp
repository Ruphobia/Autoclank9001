#include "landscape_fragmentation_analyzer.hpp"

namespace landscape_fragmentation_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Landscape fragmentation analyzer (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
