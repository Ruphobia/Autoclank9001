#include "frame_geometry_comparator.hpp"

namespace frame_geometry_comparator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Frame Geometry Comparator (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
