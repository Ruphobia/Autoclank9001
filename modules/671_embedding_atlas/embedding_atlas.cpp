#include "embedding_atlas.hpp"

namespace embedding_atlas {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Embedding atlas (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
