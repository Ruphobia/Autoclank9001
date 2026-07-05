#include "magnetics_designer.hpp"

namespace magnetics_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Magnetics designer (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
