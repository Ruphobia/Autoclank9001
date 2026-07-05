#include "shot_list_builder.hpp"

namespace shot_list_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Shot List Builder (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
