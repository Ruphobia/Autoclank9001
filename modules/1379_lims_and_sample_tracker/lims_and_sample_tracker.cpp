#include "lims_and_sample_tracker.hpp"

namespace lims_and_sample_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: LIMS and sample tracker (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
