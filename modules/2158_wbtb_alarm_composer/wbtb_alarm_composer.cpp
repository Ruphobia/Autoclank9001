#include "wbtb_alarm_composer.hpp"

namespace wbtb_alarm_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: WBTB Alarm Composer (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
