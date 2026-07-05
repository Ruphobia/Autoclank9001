#include "hash_workbench.hpp"

namespace hash_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hash Workbench (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
