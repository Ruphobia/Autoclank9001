#include "manuscript_outliner.hpp"

namespace manuscript_outliner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Manuscript outliner (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
