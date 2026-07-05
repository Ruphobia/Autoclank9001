#include "marginalia_notebook.hpp"

namespace marginalia_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Marginalia Notebook (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
