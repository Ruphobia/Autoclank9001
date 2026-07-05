#include "readability_analyzer.hpp"

namespace readability_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Readability analyzer (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
