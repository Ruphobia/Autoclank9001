#include "lens_database_browser.hpp"

namespace lens_database_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lens Database Browser (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
