#include "consensus_sleep_diary.hpp"

namespace consensus_sleep_diary {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Consensus sleep diary (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
