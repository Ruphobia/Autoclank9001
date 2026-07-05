#include "endogamy_and_pedigree_collapse_detector.hpp"

namespace endogamy_and_pedigree_collapse_detector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Endogamy and Pedigree Collapse Detector (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
