#include "export_control_screener.hpp"

namespace export_control_screener {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Export Control Screener (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
