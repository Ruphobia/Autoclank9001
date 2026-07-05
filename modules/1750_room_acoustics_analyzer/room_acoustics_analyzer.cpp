#include "room_acoustics_analyzer.hpp"

namespace room_acoustics_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Room acoustics analyzer (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
