#include "catalog_browser.hpp"

namespace catalog_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Catalog Browser (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
