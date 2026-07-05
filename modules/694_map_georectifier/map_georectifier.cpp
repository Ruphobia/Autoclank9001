#include "map_georectifier.hpp"

namespace map_georectifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Map georectifier (History and archival research). Awaits wire-up.";
    return s;
}

}
