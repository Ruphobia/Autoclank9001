#include "seismic_seg_y_previewer.hpp"

namespace seismic_seg_y_previewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Seismic SEG-Y previewer (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
