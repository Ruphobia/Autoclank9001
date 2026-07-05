#include "score_notation_editor.hpp"

namespace score_notation_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Score Notation Editor (Music composition and theory). Awaits wire-up.";
    return s;
}

}
