#include "argument_map_editor.hpp"

namespace argument_map_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Argument map editor (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
