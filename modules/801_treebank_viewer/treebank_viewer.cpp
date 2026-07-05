#include "treebank_viewer.hpp"

namespace treebank_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Treebank Viewer (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
