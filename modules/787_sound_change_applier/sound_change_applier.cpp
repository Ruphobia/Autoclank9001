#include "sound_change_applier.hpp"

namespace sound_change_applier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sound Change Applier (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
