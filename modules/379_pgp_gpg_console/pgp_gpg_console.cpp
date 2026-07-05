#include "pgp_gpg_console.hpp"

namespace pgp_gpg_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PGP/GPG Console (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
