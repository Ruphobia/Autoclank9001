#include "sgf_library_indexer.hpp"

namespace sgf_library_indexer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SGF library indexer (Game playing assistants). Awaits wire-up.";
    return s;
}

}
