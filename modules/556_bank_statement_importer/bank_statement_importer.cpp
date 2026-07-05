#include "bank_statement_importer.hpp"

namespace bank_statement_importer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bank Statement Importer (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
