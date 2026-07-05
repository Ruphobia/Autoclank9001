#include "replay_annotator.hpp"

namespace replay_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Replay annotator (Game playing assistants). Awaits wire-up.";
    return s;
}

}
