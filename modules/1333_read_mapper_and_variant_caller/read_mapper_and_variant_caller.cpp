#include "read_mapper_and_variant_caller.hpp"

namespace read_mapper_and_variant_caller {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Read Mapper and Variant Caller (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
