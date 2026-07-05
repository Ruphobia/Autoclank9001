#include "vaccine_and_prophylaxis_scheduler.hpp"

namespace vaccine_and_prophylaxis_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vaccine and prophylaxis scheduler (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
