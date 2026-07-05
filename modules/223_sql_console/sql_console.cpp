#include "sql_console.hpp"

namespace sql_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SQL console (Databases). Awaits wire-up.";
    return s;
}

}
