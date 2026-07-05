#include "cron_and_timer_editor.hpp"

namespace cron_and_timer_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cron and timer editor (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
