#include "key_inspector.hpp"

namespace key_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Key Inspector (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
