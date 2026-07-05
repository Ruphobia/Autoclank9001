#include "hrv_and_biofeedback_viewer.hpp"

namespace hrv_and_biofeedback_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HRV and biofeedback viewer (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
