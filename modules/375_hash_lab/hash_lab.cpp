#include "hash_lab.hpp"

namespace hash_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hash Lab (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
