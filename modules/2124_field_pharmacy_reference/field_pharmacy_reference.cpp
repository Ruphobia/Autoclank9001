#include "field_pharmacy_reference.hpp"

namespace field_pharmacy_reference {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Field pharmacy reference (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
