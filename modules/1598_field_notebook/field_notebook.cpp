#include "field_notebook.hpp"

namespace field_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Field Notebook (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
