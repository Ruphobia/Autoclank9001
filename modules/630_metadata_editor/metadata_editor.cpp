#include "metadata_editor.hpp"

namespace metadata_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Metadata editor (Documents and reading). Awaits wire-up.";
    return s;
}

}
