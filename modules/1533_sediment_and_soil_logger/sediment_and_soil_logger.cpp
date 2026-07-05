#include "sediment_and_soil_logger.hpp"

namespace sediment_and_soil_logger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sediment and soil logger (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
