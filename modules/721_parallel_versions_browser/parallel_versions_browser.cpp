#include "parallel_versions_browser.hpp"

namespace parallel_versions_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Parallel Versions Browser (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
