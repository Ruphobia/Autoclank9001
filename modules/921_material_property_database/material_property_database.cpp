#include "material_property_database.hpp"

namespace material_property_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Material property database (Physics and math). Awaits wire-up.";
    return s;
}

}
