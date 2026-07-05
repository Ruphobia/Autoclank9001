#include "harris_matrix_editor.hpp"

namespace harris_matrix_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Harris matrix editor (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
