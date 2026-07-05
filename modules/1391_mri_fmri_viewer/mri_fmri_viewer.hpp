#pragma once

#include <string>

// Stub module for the "MRI/fMRI viewer" tool entry.
// Genomics, synthetic biology, and neuroscience.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace mri_fmri_viewer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
