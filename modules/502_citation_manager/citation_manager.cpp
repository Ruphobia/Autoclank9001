#include "citation_manager.hpp"

namespace citation_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Citation manager (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
