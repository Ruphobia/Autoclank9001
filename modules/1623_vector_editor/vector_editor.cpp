#include "vector_editor.hpp"

namespace vector_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vector editor (Image and video). Awaits wire-up.";
    return s;
}

}
