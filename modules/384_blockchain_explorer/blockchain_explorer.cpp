#include "blockchain_explorer.hpp"

namespace blockchain_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Blockchain Explorer (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
