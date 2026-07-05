#include "lighting_plot_drafter.hpp"

namespace lighting_plot_drafter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lighting Plot Drafter (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
