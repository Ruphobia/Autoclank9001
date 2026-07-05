#include "compost_recipe_builder.hpp"

namespace compost_recipe_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Compost Recipe Builder (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
