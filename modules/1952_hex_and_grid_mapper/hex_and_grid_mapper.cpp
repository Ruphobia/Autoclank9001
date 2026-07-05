#include "hex_and_grid_mapper.hpp"

namespace hex_and_grid_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hex And Grid Mapper (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
