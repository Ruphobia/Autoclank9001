#include "fluids_capacities_database.hpp"

namespace fluids_capacities_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fluids & Capacities Database (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
