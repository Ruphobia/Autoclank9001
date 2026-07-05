#include "redline_diff.hpp"

namespace redline_diff {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Redline Diff (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
