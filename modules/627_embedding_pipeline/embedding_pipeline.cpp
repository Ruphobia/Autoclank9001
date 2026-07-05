#include "embedding_pipeline.hpp"

namespace embedding_pipeline {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Embedding pipeline (Documents and reading). Awaits wire-up.";
    return s;
}

}
