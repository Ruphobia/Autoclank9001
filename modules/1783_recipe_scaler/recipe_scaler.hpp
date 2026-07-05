#pragma once

#include <string>

// Stub module for the "Recipe Scaler" tool entry.
// Cooking, brewing, fermentation, food science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace recipe_scaler {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
