#include "har_viewer_and_diff.hpp"

namespace har_viewer_and_diff {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HAR viewer and diff (Web development). Awaits wire-up.";
    return s;
}

}
