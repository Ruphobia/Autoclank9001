#include "transaction_builder.hpp"

namespace transaction_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Transaction Builder (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
