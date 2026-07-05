#include "memory_watcher.hpp"

namespace memory_watcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Memory Watcher (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
