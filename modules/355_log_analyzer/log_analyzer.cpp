#include "log_analyzer.hpp"

namespace log_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Log Analyzer (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
