#include "parasitology_bench.hpp"

namespace parasitology_bench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Parasitology bench (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
