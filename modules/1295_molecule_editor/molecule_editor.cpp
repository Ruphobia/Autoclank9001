#include "molecule_editor.hpp"

namespace molecule_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Molecule editor (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
