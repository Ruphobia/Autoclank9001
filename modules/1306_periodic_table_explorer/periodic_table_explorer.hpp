#pragma once

#include <string>

// Stub module for the "Periodic table explorer" tool entry.
// Chemistry and lab.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace periodic_table_explorer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
