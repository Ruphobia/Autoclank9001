#include "hdr_merger.hpp"

namespace hdr_merger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HDR Merger (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
