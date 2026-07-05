#include "prosopography_database.hpp"

namespace prosopography_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Prosopography database (History and archival research). Awaits wire-up.";
    return s;
}

}
