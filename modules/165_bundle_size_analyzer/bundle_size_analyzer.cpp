#include "bundle_size_analyzer.hpp"

namespace bundle_size_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bundle size analyzer (Mobile development). Awaits wire-up.";
    return s;
}

}
