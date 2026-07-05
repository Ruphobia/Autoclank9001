#include "critical_apparatus_editor.hpp"

namespace critical_apparatus_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Critical Apparatus Editor (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
