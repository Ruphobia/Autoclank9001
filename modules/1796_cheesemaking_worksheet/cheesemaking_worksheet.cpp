#include "cheesemaking_worksheet.hpp"

namespace cheesemaking_worksheet {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cheesemaking Worksheet (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
