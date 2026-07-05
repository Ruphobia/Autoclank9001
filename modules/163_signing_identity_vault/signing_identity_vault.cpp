#include "signing_identity_vault.hpp"

namespace signing_identity_vault {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Signing identity vault (Mobile development). Awaits wire-up.";
    return s;
}

}
