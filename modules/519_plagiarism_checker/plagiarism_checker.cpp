#include "plagiarism_checker.hpp"

namespace plagiarism_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Plagiarism Checker (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
