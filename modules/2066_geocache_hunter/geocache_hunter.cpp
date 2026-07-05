#include "geocache_hunter.hpp"

namespace geocache_hunter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Geocache Hunter (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
