#include "marine_species_id.hpp"

namespace marine_species_id {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Marine species ID (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
