#include "computer_vision_pipeline.hpp"

namespace computer_vision_pipeline {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Computer vision pipeline (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
