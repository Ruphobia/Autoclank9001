#include "eeg_meg_analyzer.hpp"

namespace eeg_meg_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: EEG/MEG analyzer (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
