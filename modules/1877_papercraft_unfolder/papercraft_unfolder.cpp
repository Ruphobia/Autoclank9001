#include "papercraft_unfolder.hpp"

namespace papercraft_unfolder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Papercraft unfolder (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
