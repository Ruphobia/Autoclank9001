#include "notam_and_tfr_browser.hpp"

namespace notam_and_tfr_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NOTAM and TFR browser (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
