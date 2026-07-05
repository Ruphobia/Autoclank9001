#include "log_aggregator.hpp"

namespace log_aggregator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Log aggregator (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
