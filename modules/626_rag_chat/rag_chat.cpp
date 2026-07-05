#include "rag_chat.hpp"

namespace rag_chat {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RAG chat (Documents and reading). Awaits wire-up.";
    return s;
}

}
