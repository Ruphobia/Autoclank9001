#pragma once

#include <string>

// Stub module for the "Antiques Provenance and Appraisal Workbench" tool entry.
// Numismatics, philately, collecting, antiques.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace antiques_provenance_and_appraisal_workbench {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
