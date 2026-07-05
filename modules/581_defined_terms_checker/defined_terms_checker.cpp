#include "defined_terms_checker.hpp"

namespace defined_terms_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Defined Terms Checker (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
