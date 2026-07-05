#pragma once

#include <string>

// Stub module for the "Variant Annotator and Interpreter" tool entry.
// Biology and bioinformatics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace variant_annotator_and_interpreter {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
