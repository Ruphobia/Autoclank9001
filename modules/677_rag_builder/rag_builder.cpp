#include "rag_builder.hpp"

namespace rag_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RAG builder (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
