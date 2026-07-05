#include "paleography_htr.hpp"

namespace paleography_htr {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Paleography HTR (History and archival research). Awaits wire-up.";
    return s;
}

}
