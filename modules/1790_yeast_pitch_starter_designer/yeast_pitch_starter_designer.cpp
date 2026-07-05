#include "yeast_pitch_starter_designer.hpp"

namespace yeast_pitch_starter_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Yeast Pitch & Starter Designer (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
