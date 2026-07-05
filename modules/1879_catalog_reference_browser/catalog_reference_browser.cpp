#include "catalog_reference_browser.hpp"

namespace catalog_reference_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Catalog Reference Browser (Numismatics, philately, collecting, antiques). Awaits wire-up.";
    return s;
}

}
