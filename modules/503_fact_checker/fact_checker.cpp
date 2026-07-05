#include "fact_checker.hpp"

namespace fact_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fact-checker (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
