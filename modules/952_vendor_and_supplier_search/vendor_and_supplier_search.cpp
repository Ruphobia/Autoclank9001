#include "vendor_and_supplier_search.hpp"

namespace vendor_and_supplier_search {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vendor and supplier search (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
