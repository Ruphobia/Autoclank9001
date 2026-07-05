#include "robot_description_editor.hpp"

namespace robot_description_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Robot description editor (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
