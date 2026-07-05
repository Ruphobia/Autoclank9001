#include "import_export_hub.hpp"

namespace import_export_hub {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Import/Export hub (Databases). Awaits wire-up.";
    return s;
}

}
