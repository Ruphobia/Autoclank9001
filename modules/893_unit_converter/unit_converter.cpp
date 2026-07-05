#include "unit_converter.hpp"

namespace unit_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Unit converter (Physics and math). Awaits wire-up.";
    return s;
}

}
