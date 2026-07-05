#include "cam_toolpath_generator.hpp"

namespace cam_toolpath_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CAM toolpath generator (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
