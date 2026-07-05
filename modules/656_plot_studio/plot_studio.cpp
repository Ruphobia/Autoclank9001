#include "plot_studio.hpp"

namespace plot_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Plot studio (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
