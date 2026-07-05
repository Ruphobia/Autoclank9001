#include "play_by_play_loader.hpp"

namespace play_by_play_loader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Play-by-play loader (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
