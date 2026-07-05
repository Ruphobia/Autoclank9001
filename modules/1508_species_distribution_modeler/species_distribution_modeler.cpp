#include "species_distribution_modeler.hpp"

namespace species_distribution_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Species distribution modeler (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
