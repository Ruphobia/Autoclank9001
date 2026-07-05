#include "schema_browser.hpp"

namespace schema_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Schema browser (Databases). Awaits wire-up.";
    return s;
}

}
