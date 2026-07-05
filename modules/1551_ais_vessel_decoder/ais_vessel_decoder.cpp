#include "ais_vessel_decoder.hpp"

namespace ais_vessel_decoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: AIS vessel decoder (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
