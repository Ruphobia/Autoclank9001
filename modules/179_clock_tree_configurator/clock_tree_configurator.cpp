#include "clock_tree_configurator.hpp"

namespace clock_tree_configurator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Clock tree configurator (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
