#include "recall_tsb_watcher.hpp"

namespace recall_tsb_watcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Recall & TSB Watcher (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
