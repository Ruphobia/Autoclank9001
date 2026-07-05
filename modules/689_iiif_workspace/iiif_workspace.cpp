#include "iiif_workspace.hpp"

namespace iiif_workspace {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: IIIF workspace (History and archival research). Awaits wire-up.";
    return s;
}

}
