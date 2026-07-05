#pragma once

#include <string>

// Stub module for the "Compost Recipe Builder" tool entry.
// Agriculture and horticulture.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace compost_recipe_builder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
