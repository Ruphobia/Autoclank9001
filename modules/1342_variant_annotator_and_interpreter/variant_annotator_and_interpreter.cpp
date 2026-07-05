#include "variant_annotator_and_interpreter.hpp"

namespace variant_annotator_and_interpreter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Variant Annotator and Interpreter (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
