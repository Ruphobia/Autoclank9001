#include "load_table_browser.hpp"

namespace load_table_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Load Table Browser (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
