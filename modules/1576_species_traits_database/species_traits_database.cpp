#include "species_traits_database.hpp"

namespace species_traits_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Species traits database (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
