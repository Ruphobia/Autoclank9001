#include "search_index_manager.hpp"

namespace search_index_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Search index manager (Databases). Awaits wire-up.";
    return s;
}

}
