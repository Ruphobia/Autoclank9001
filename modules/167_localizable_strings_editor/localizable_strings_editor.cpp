#include "localizable_strings_editor.hpp"

namespace localizable_strings_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Localizable strings editor (Mobile development). Awaits wire-up.";
    return s;
}

}
