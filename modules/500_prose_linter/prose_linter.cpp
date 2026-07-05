#include "prose_linter.hpp"

namespace prose_linter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Prose linter (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
