#include "frame_interpolator.hpp"

namespace frame_interpolator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Frame interpolator (Image and video). Awaits wire-up.";
    return s;
}

}
