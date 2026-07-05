#include "comparative_religion_timeline.hpp"

namespace comparative_religion_timeline {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Comparative-religion timeline (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
