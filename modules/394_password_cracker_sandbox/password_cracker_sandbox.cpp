#include "password_cracker_sandbox.hpp"

namespace password_cracker_sandbox {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Password Cracker Sandbox (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
