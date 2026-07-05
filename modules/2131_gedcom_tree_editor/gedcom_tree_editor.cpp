#include "gedcom_tree_editor.hpp"

namespace gedcom_tree_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GEDCOM Tree Editor (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
