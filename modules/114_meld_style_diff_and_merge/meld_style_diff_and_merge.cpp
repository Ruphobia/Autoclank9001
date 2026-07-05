#include "meld_style_diff_and_merge.hpp"

namespace meld_style_diff_and_merge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Meld-style diff and merge (Per-language toolchains). Awaits wire-up.";
    return s;
}

}
