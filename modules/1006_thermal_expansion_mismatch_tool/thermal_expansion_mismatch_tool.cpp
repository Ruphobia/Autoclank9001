#include "thermal_expansion_mismatch_tool.hpp"

namespace thermal_expansion_mismatch_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Thermal Expansion Mismatch Tool (Materials science and metallurgy). Awaits wire-up.";
    return s;
}

}
