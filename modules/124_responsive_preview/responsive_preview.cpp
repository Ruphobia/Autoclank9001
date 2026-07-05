#include "responsive_preview.hpp"

namespace responsive_preview {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Responsive preview (Web development). Awaits wire-up.";
    return s;
}

}
