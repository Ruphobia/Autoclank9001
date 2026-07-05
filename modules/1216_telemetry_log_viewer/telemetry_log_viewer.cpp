#include "telemetry_log_viewer.hpp"

namespace telemetry_log_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Telemetry Log Viewer (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
