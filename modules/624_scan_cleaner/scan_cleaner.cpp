#include "scan_cleaner.hpp"

namespace scan_cleaner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Scan cleaner (Documents and reading). Awaits wire-up.";
    return s;
}

}
