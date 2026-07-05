#include "vector_index_lab.hpp"

namespace vector_index_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vector index lab (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
