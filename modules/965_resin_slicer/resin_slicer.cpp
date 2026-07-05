#include "resin_slicer.hpp"

namespace resin_slicer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Resin Slicer (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
