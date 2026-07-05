#include "visual_search.hpp"

namespace visual_search {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Visual search (Image and video). Awaits wire-up.";
    return s;
}

}
