#include "recipe_scaler.hpp"

namespace recipe_scaler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Recipe Scaler (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
