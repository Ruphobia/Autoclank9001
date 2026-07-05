#include "reverse_outliner.hpp"

namespace reverse_outliner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reverse outliner (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
