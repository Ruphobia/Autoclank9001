#include "acoustic_species_id.hpp"

namespace acoustic_species_id {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Acoustic species ID (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
