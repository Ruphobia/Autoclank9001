#include "contract_drafter.hpp"

namespace contract_drafter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Contract Drafter (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
