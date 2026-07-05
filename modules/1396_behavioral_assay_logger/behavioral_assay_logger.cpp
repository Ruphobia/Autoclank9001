#include "behavioral_assay_logger.hpp"

namespace behavioral_assay_logger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Behavioral assay logger (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
