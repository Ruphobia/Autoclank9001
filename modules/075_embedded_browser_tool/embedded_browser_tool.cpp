#include "embedded_browser_tool.hpp"

namespace embedded_browser_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Embedded browser tool (Foundation (bootstrap)). Awaits wire-up.";
    return s;
}

}
