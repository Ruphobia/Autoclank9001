#include "weaving_draft_editor.hpp"

namespace weaving_draft_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Weaving Draft Editor (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
