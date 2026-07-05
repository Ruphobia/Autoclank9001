#include "segment_triangulator.hpp"

namespace segment_triangulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Segment Triangulator (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
