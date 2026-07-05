#include "patch_diff.hpp"

namespace patch_diff {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Patch Diff (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
