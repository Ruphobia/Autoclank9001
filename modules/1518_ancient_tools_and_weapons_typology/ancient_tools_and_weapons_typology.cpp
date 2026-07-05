#include "ancient_tools_and_weapons_typology.hpp"

namespace ancient_tools_and_weapons_typology {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ancient tools and weapons typology (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
