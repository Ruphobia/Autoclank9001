#pragma once

#include <string>

// Stub module for the "Project and shop scheduler" tool entry.
// Mechanical and manufacturing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace project_and_shop_scheduler {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
