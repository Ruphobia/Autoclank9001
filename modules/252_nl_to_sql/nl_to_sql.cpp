#include "nl_to_sql.hpp"

namespace nl_to_sql {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NL-to-SQL (Databases). Awaits wire-up.";
    return s;
}

}
