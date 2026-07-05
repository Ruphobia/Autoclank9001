#include "historical_units_converter.hpp"

namespace historical_units_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Historical units converter (History and archival research). Awaits wire-up.";
    return s;
}

}
