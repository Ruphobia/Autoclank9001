#include "contest_logger.hpp"

namespace contest_logger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Contest logger (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
