#include "stable_isotope_analyzer.hpp"

namespace stable_isotope_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stable isotope analyzer (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
