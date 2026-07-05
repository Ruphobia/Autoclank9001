#pragma once

#include <string>

// Stub module for the "AIS decoder and viewer" tool entry.
// Sailing, boating, navigation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace ais_decoder_and_viewer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
