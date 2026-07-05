#include "user_and_permission_editor.hpp"

namespace user_and_permission_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: User and permission editor (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
