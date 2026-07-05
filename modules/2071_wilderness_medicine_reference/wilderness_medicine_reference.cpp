#include "wilderness_medicine_reference.hpp"

namespace wilderness_medicine_reference {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wilderness Medicine Reference (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
