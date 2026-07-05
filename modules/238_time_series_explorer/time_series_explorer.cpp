#include "time_series_explorer.hpp"

namespace time_series_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Time-series explorer (Databases). Awaits wire-up.";
    return s;
}

}
