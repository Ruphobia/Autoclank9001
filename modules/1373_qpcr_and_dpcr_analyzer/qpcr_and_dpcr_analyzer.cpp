#include "qpcr_and_dpcr_analyzer.hpp"

namespace qpcr_and_dpcr_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: qPCR and dPCR analyzer (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
