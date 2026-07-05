#pragma once

#include <string>

// Stub module for the "Trip Journal & Photo Geotagger" tool entry.
// Outdoor recreation: camping, climbing, skiing, paragliding, geocaching.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace trip_journal_photo_geotagger {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
