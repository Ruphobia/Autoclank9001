#include "crash_log_symbolicator.hpp"

namespace crash_log_symbolicator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Crash log symbolicator (Mobile development). Awaits wire-up.";
    return s;
}

}
