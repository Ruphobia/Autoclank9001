#include "ssh_toolkit.hpp"

namespace ssh_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SSH Toolkit (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
