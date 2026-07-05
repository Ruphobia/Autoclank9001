#include "watermark_and_provenance.hpp"

namespace watermark_and_provenance {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Watermark and provenance (Image and video). Awaits wire-up.";
    return s;
}

}
