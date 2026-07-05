#include "earthquake_feed.hpp"

namespace earthquake_feed {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Earthquake Feed (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
