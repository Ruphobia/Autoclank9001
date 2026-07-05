#include "mri_fmri_viewer.hpp"

namespace mri_fmri_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MRI/fMRI viewer (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
