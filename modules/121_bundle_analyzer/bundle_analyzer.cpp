#include "bundle_analyzer.hpp"

namespace bundle_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bundle analyzer (Web development). Awaits wire-up.";
    return s;
}

}
