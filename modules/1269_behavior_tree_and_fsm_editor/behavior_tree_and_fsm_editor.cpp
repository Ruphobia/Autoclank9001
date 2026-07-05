#include "behavior_tree_and_fsm_editor.hpp"

namespace behavior_tree_and_fsm_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Behavior tree and FSM editor (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
