#include "mcmaster_carr_lookup.hpp"

namespace mcmaster_carr_lookup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: McMaster-Carr lookup (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
