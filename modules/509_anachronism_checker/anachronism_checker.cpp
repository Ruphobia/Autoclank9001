#include "anachronism_checker.hpp"

namespace anachronism_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Anachronism checker (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
