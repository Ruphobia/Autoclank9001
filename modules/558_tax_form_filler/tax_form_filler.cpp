#include "tax_form_filler.hpp"

namespace tax_form_filler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tax Form Filler (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
