#include "stored_procedure_ide.hpp"

namespace stored_procedure_ide {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stored procedure IDE (Databases). Awaits wire-up.";
    return s;
}

}
