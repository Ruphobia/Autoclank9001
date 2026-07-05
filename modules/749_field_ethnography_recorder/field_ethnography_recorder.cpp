#include "field_ethnography_recorder.hpp"

namespace field_ethnography_recorder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Field Ethnography Recorder (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
