#include "crop_rotation_planner.hpp"

namespace crop_rotation_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Crop Rotation Planner (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
