#include "bibtex_csl_manager.hpp"

namespace bibtex_csl_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BibTeX/CSL Manager (Academic and scholarly research). Awaits wire-up.";
    return s;
}

}
