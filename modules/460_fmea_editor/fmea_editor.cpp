#include "fmea_editor.hpp"

namespace fmea_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FMEA editor (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
