#include "abi_calldata_decoder.hpp"

namespace abi_calldata_decoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ABI & Calldata Decoder (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
