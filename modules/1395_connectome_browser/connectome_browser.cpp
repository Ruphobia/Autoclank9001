#include "connectome_browser.hpp"

namespace connectome_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Connectome browser (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
