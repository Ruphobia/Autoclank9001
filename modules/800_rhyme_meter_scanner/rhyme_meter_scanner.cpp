#include "rhyme_meter_scanner.hpp"

namespace rhyme_meter_scanner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rhyme & Meter Scanner (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
