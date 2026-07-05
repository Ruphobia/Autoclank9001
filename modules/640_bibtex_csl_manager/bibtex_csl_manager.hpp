#pragma once

#include <string>

// Stub module for the "BibTeX/CSL Manager" tool entry.
// Academic and scholarly research.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace bibtex_csl_manager {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
