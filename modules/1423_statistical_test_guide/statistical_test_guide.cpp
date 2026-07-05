#include "statistical_test_guide.hpp"

namespace statistical_test_guide {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Statistical test guide (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
