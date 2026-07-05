#include "patent_drafter.hpp"

namespace patent_drafter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Patent drafter (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
