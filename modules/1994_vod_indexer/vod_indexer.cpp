#include "vod_indexer.hpp"

namespace vod_indexer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: VOD Indexer (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
