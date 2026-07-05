#include "face_tools.hpp"

namespace face_tools {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Face tools (Image and video). Awaits wire-up.";
    return s;
}

}
