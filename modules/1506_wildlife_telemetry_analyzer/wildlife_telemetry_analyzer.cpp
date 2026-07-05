#include "wildlife_telemetry_analyzer.hpp"

namespace wildlife_telemetry_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wildlife telemetry analyzer (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
