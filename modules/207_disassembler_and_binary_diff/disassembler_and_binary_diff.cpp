#include "disassembler_and_binary_diff.hpp"

namespace disassembler_and_binary_diff {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Disassembler and binary diff (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
