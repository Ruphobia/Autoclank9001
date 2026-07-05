#include "patient_case_notebook.hpp"

namespace patient_case_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Patient Case Notebook (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
