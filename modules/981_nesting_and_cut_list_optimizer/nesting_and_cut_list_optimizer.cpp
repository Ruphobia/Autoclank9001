#include "nesting_and_cut_list_optimizer.hpp"

namespace nesting_and_cut_list_optimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Nesting and Cut List Optimizer (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
