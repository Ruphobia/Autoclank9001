#pragma once

#include <string>

// Stub module for the "Kiln schedule planner" tool entry.
// Visual arts: calligraphy, painting, sculpture.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace kiln_schedule_planner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
