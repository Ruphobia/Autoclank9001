#include "loudness_and_metering_suite.hpp"

namespace loudness_and_metering_suite {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Loudness and metering suite (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
