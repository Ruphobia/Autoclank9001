#include "sun_path_shading_analyzer.hpp"

namespace sun_path_shading_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sun Path & Shading Analyzer (Energy systems). Awaits wire-up.";
    return s;
}

}
