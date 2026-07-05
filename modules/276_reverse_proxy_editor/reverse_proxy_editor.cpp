#include "reverse_proxy_editor.hpp"

namespace reverse_proxy_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reverse proxy editor (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
