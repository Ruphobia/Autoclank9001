#include "firewall_rule_editor.hpp"

namespace firewall_rule_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Firewall rule editor (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
