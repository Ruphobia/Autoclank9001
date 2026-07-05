#include "interlinear_gloss_editor.hpp"

namespace interlinear_gloss_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Interlinear Gloss Editor (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
