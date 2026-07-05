#include "key_forge.hpp"

namespace key_forge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Key Forge (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
