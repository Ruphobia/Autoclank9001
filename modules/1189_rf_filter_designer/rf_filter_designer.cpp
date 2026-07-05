#include "rf_filter_designer.hpp"

namespace rf_filter_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RF filter designer (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
