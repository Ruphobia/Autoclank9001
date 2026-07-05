#pragma once

#include <string>

// Stub module for the "Repair Notebook" tool entry.
// Vintage hardware preservation and retrocomputing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace repair_notebook {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
