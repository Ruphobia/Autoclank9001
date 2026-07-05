#include "tolerance_stackup_analyzer.hpp"

namespace tolerance_stackup_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tolerance stackup analyzer (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
