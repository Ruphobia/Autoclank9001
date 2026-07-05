#include "fermentationlog.hpp"

namespace fermentationlog {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FermentationLog (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
