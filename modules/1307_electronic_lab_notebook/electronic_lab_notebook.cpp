#include "electronic_lab_notebook.hpp"

namespace electronic_lab_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Electronic lab notebook (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
