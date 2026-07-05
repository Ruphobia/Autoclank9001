#include "codebook_manager.hpp"

namespace codebook_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Codebook manager (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
