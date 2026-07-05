#include "regional_map_sculptor.hpp"

namespace regional_map_sculptor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Regional Map Sculptor (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
