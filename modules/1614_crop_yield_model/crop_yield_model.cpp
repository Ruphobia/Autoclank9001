#include "crop_yield_model.hpp"

namespace crop_yield_model {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Crop Yield Model (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
