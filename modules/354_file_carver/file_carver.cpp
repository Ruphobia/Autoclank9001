#include "file_carver.hpp"

namespace file_carver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: File Carver (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
