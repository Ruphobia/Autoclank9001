#include "console_syntax_editor.hpp"

namespace console_syntax_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Console Syntax Editor (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
