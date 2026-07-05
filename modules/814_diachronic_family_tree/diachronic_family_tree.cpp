#include "diachronic_family_tree.hpp"

namespace diachronic_family_tree {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Diachronic Family Tree (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
