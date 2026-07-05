#include "linked_notebook.hpp"

namespace linked_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Linked notebook (Office productivity). Awaits wire-up.";
    return s;
}

}
