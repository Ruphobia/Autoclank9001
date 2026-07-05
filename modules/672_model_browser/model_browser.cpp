#include "model_browser.hpp"

namespace model_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Model browser (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
