#include "walkthrough_wiki_indexer.hpp"

namespace walkthrough_wiki_indexer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Walkthrough/wiki indexer (Game playing assistants). Awaits wire-up.";
    return s;
}

}
