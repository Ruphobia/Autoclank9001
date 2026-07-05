#include "snapshot_test_diff_viewer.hpp"

namespace snapshot_test_diff_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Snapshot test diff viewer (Mobile development). Awaits wire-up.";
    return s;
}

}
