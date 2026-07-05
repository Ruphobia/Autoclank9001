#include "disassembler_decompiler_viewer.hpp"

namespace disassembler_decompiler_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Disassembler/Decompiler Viewer (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
