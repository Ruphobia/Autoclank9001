#include "latex_formula_editor.hpp"

namespace latex_formula_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: LaTeX formula editor (Physics and math). Awaits wire-up.";
    return s;
}

}
