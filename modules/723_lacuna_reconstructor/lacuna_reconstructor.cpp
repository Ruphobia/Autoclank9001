#include "lacuna_reconstructor.hpp"

namespace lacuna_reconstructor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lacuna Reconstructor (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
