#include "ebook_compiler.hpp"

namespace ebook_compiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ebook compiler (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
