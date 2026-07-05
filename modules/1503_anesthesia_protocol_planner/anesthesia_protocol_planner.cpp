#include "anesthesia_protocol_planner.hpp"

namespace anesthesia_protocol_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Anesthesia protocol planner (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
