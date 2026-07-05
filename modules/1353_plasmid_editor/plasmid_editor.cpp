#include "plasmid_editor.hpp"

namespace plasmid_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Plasmid editor (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
