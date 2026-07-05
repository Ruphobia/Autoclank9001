#include "restriction_enzyme_mapper.hpp"

namespace restriction_enzyme_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Restriction Enzyme Mapper (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
