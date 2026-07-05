#include "genealogy_and_gedcom_editor.hpp"

namespace genealogy_and_gedcom_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Genealogy and GEDCOM editor (History and archival research). Awaits wire-up.";
    return s;
}

}
