#include "chromatography_method_designer.hpp"

namespace chromatography_method_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Chromatography method designer (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
