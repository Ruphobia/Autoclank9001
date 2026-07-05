#include "evm_debugger.hpp"

namespace evm_debugger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: EVM Debugger (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
