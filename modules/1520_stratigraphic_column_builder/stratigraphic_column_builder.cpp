#include "stratigraphic_column_builder.hpp"

namespace stratigraphic_column_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stratigraphic column builder (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
