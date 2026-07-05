#include "relativity_playground.hpp"

namespace relativity_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Relativity playground (Physics and math). Awaits wire-up.";
    return s;
}

}
