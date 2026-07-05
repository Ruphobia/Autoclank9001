#include "celestial_sight_reducer.hpp"

namespace celestial_sight_reducer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Celestial sight reducer (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
