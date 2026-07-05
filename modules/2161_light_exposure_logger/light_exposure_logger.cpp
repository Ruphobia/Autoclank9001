#include "light_exposure_logger.hpp"

namespace light_exposure_logger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Light Exposure Logger (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
