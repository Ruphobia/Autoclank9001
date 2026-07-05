#include "embedded_sqlite_duckdb_workbench.hpp"

namespace embedded_sqlite_duckdb_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Embedded SQLite/DuckDB workbench (Databases). Awaits wire-up.";
    return s;
}

}
