#include "litigation_hold_manager.hpp"

namespace litigation_hold_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Litigation Hold Manager (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
