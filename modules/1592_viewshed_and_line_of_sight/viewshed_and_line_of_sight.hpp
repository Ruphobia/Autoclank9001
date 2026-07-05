#pragma once

#include <string>

// Stub module for the "Viewshed and Line-of-Sight" tool entry.
// Geology, GIS, and earth sciences.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace viewshed_and_line_of_sight {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
