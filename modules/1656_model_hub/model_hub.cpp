#include "model_hub.hpp"

namespace model_hub {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Model hub (Image and video). Awaits wire-up.";
    return s;
}

}
