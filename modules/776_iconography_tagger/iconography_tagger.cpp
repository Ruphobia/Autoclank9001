#include "iconography_tagger.hpp"

namespace iconography_tagger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Iconography Tagger (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
