#include "anchor_watch.hpp"

namespace anchor_watch {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Anchor watch (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
