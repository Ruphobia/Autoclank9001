#include "metagenomics_classifier.hpp"

namespace metagenomics_classifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Metagenomics Classifier (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
