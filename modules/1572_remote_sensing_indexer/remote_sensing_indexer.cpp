#include "remote_sensing_indexer.hpp"

namespace remote_sensing_indexer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Remote sensing indexer (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
