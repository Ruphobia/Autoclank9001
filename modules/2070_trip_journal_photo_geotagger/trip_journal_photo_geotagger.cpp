#include "trip_journal_photo_geotagger.hpp"

namespace trip_journal_photo_geotagger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Trip Journal & Photo Geotagger (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
