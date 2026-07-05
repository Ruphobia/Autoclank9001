#include "orthography_mapper.hpp"

namespace orthography_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Orthography Mapper (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
