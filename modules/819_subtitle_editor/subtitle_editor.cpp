#include "subtitle_editor.hpp"

namespace subtitle_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Subtitle editor (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
