#include "secrets_vault.hpp"

namespace secrets_vault {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Secrets vault (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
