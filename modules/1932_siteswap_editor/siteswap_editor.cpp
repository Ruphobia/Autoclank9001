#include "siteswap_editor.hpp"

namespace siteswap_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Siteswap Editor (Magic, juggling, stage illusion, performance). Awaits wire-up.";
    return s;
}

}
