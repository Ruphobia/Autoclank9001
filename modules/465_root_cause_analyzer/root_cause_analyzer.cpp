#include "root_cause_analyzer.hpp"

namespace root_cause_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Root-cause analyzer (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
