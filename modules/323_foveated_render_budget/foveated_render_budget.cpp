#include "foveated_render_budget.hpp"

namespace foveated_render_budget {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Foveated Render Budget (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
