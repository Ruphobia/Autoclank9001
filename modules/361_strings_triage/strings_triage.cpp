#include "strings_triage.hpp"

namespace strings_triage {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Strings Triage (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
