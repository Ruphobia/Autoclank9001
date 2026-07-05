#include "constellation_designer.hpp"

namespace constellation_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Constellation Designer (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
