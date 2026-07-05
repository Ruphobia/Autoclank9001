#include "geophysics_survey_processor.hpp"

namespace geophysics_survey_processor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Geophysics survey processor (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
