#include "cuppingsession.hpp"

namespace cuppingsession {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CuppingSession (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
