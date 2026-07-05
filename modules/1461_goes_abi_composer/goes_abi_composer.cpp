#include "goes_abi_composer.hpp"

namespace goes_abi_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GOES ABI composer (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
