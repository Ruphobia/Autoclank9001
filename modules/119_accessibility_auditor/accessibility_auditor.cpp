#include "accessibility_auditor.hpp"

namespace accessibility_auditor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Accessibility auditor (Web development). Awaits wire-up.";
    return s;
}

}
