#include "sleep_stage_logger.hpp"

namespace sleep_stage_logger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sleep Stage Logger (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
