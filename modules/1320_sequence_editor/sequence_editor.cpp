#include "sequence_editor.hpp"

namespace sequence_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sequence Editor (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
