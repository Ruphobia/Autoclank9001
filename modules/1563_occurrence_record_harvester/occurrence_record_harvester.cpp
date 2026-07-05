#include "occurrence_record_harvester.hpp"

namespace occurrence_record_harvester {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Occurrence record harvester (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
