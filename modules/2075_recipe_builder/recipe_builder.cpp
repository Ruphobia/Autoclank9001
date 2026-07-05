#include "recipe_builder.hpp"

namespace recipe_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Recipe builder (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
