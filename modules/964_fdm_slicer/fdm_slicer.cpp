#include "fdm_slicer.hpp"

namespace fdm_slicer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FDM Slicer (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
