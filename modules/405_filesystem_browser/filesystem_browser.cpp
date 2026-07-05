#include "filesystem_browser.hpp"

namespace filesystem_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Filesystem Browser (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
