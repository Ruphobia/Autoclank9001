#include "first_aid_decision_tree.hpp"

namespace first_aid_decision_tree {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: First-aid decision tree (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
