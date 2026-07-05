#include "forge_and_kiln_build_calculator.hpp"

namespace forge_and_kiln_build_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Forge and Kiln Build Calculator (Traditional crafts: horology, blacksmithing, jewelry, glassblowing, pottery, leatherwork, cobbling). Awaits wire-up.";
    return s;
}

}
