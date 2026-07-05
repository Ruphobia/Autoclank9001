#include "roadmap_visualizer.hpp"

namespace roadmap_visualizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Roadmap visualizer (Project management). Awaits wire-up.";
    return s;
}

}
