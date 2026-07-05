#include "bibliographic_search.hpp"

namespace bibliographic_search {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bibliographic search (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
