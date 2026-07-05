#include "hazop_and_lopa_workbook.hpp"

namespace hazop_and_lopa_workbook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HAZOP and LOPA workbook (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
