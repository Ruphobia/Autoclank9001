#include "grading_guide.hpp"

namespace grading_guide {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Grading Guide (Numismatics, philately, collecting, antiques). Awaits wire-up.";
    return s;
}

}
