#include "link_budget_calculator.hpp"

namespace link_budget_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Link Budget Calculator (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
