#include "mortality_table_browser.hpp"

namespace mortality_table_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mortality table browser (Real estate, insurance, actuarial). Awaits wire-up.";
    return s;
}

}
