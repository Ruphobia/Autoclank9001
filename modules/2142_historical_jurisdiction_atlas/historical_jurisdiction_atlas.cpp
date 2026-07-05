#include "historical_jurisdiction_atlas.hpp"

namespace historical_jurisdiction_atlas {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Historical Jurisdiction Atlas (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
