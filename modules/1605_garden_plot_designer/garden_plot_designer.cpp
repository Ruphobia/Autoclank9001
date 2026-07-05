#include "garden_plot_designer.hpp"

namespace garden_plot_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Garden Plot Designer (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
