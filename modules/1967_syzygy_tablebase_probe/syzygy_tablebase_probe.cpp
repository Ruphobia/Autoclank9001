#include "syzygy_tablebase_probe.hpp"

namespace syzygy_tablebase_probe {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Syzygy tablebase probe (Game playing assistants). Awaits wire-up.";
    return s;
}

}
