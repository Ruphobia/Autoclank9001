#pragma once

#include <string>

// Stub module for the "Gradient designer" tool entry.
// Visual arts: calligraphy, painting, sculpture.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace gradient_designer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
