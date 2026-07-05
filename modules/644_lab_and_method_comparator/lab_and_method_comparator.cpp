#include "lab_and_method_comparator.hpp"

namespace lab_and_method_comparator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lab and Method Comparator (Academic and scholarly research). Awaits wire-up.";
    return s;
}

}
