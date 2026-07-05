#include "journal_browser.hpp"

namespace journal_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Journal browser (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
