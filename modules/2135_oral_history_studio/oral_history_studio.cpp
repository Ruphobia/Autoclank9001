#include "oral_history_studio.hpp"

namespace oral_history_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Oral History Studio (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
