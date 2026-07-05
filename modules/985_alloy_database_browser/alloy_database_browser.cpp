#include "alloy_database_browser.hpp"

namespace alloy_database_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Alloy Database Browser (Materials science and metallurgy). Awaits wire-up.";
    return s;
}

}
