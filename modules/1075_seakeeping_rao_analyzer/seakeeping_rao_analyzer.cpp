#include "seakeeping_rao_analyzer.hpp"

namespace seakeeping_rao_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Seakeeping RAO analyzer (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
