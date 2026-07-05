#include "lightning_density_mapper.hpp"

namespace lightning_density_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lightning density mapper (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
