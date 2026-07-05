#include "scada_timeseries_viewer.hpp"

namespace scada_timeseries_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SCADA timeseries viewer (Civil utility (water, waste, sanitation)). Awaits wire-up.";
    return s;
}

}
