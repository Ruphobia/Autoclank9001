#include "atu_motif_index_browser.hpp"

namespace atu_motif_index_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ATU Motif Index Browser (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
