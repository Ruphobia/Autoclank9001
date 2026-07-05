#include "beerrecipedesigner.hpp"

namespace beerrecipedesigner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BeerRecipeDesigner (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
