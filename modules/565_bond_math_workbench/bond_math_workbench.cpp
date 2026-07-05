#include "bond_math_workbench.hpp"

namespace bond_math_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bond Math Workbench (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
