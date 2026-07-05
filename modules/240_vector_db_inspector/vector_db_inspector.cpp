#include "vector_db_inspector.hpp"

namespace vector_db_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vector DB inspector (Databases). Awaits wire-up.";
    return s;
}

}
