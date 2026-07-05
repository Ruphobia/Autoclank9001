#include "pdf_ingestor.hpp"

namespace pdf_ingestor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PDF Ingestor (Memory and knowledge accrual). Awaits wire-up.";
    return s;
}

}
