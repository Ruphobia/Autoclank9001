#include "symbol_and_call_graph_explorer.hpp"

namespace symbol_and_call_graph_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Symbol and call graph explorer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
