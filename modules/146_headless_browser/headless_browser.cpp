#include "headless_browser.hpp"

namespace headless_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Headless browser (Web development). Awaits wire-up.";
    return s;
}

}
