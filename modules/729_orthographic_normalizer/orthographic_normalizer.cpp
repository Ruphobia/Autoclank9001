#include "orthographic_normalizer.hpp"

namespace orthographic_normalizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Orthographic Normalizer (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
