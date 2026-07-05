#include "graph_db_browser.hpp"

namespace graph_db_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Graph DB browser (Databases). Awaits wire-up.";
    return s;
}

}
