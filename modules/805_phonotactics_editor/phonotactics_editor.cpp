#include "phonotactics_editor.hpp"

namespace phonotactics_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Phonotactics Editor (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
