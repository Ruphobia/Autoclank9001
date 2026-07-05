#include "replay_scrubber.hpp"

namespace replay_scrubber {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Replay Scrubber (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
