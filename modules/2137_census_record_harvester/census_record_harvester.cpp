#include "census_record_harvester.hpp"

namespace census_record_harvester {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Census Record Harvester (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
