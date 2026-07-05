#include "replay_recorder.hpp"

namespace replay_recorder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Replay Recorder (Game development). Awaits wire-up.";
    return s;
}

}
