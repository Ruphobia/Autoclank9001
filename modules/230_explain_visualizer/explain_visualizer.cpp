#include "explain_visualizer.hpp"

namespace explain_visualizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: EXPLAIN visualizer (Databases). Awaits wire-up.";
    return s;
}

}
