#include "fpv_setup_and_video_link.hpp"

namespace fpv_setup_and_video_link {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FPV setup and video link (Radio-control modeling (planes, cars, trucks, boats, drones)). Awaits wire-up.";
    return s;
}

}
