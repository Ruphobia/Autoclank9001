#include "behavior_tree_authoring.hpp"

namespace behavior_tree_authoring {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Behavior Tree Authoring (Game development). Awaits wire-up.";
    return s;
}

}
