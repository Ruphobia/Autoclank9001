#include "lifting_line_analyzer.hpp"

namespace lifting_line_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lifting Line Analyzer (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
