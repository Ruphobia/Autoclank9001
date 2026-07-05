#include "emulator_ram_watcher.hpp"

namespace emulator_ram_watcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Emulator RAM watcher (Game playing assistants). Awaits wire-up.";
    return s;
}

}
