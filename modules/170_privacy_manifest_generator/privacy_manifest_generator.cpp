#include "privacy_manifest_generator.hpp"

namespace privacy_manifest_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Privacy manifest generator (Mobile development). Awaits wire-up.";
    return s;
}

}
