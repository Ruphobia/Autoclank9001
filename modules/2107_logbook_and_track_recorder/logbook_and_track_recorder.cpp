#include "logbook_and_track_recorder.hpp"

namespace logbook_and_track_recorder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Logbook and track recorder (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
