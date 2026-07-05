#include "kombucha_scoby_tracker.hpp"

namespace kombucha_scoby_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Kombucha SCOBY Tracker (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
