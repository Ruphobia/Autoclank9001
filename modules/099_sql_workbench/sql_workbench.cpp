#include "sql_workbench.hpp"

namespace sql_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SQL workbench (Software development). Awaits wire-up.";
    return s;
}

}
