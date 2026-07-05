#include "jwt_token_workbench.hpp"

namespace jwt_token_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: JWT & Token Workbench (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
