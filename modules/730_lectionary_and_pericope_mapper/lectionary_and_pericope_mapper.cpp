#include "lectionary_and_pericope_mapper.hpp"

namespace lectionary_and_pericope_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lectionary and Pericope Mapper (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
