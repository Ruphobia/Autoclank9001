#include "ui_flow_recorder.hpp"

namespace ui_flow_recorder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: UI flow recorder (Mobile development). Awaits wire-up.";
    return s;
}

}
