#include "cocktailworkbench.hpp"

namespace cocktailworkbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CocktailWorkbench (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
