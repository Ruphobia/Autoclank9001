#include "idf_curve_generator.hpp"

namespace idf_curve_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: IDF curve generator (Civil utility (water, waste, sanitation)). Awaits wire-up.";
    return s;
}

}
