#pragma once

#include <string>

// Stub module for the "Logbook and track recorder" tool entry.
// Sailing, boating, navigation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace logbook_and_track_recorder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
