#include "storm_chase_nowcaster.hpp"

namespace storm_chase_nowcaster {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Storm chase nowcaster (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
