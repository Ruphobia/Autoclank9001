#include "hardness_converter.hpp"

namespace hardness_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hardness Converter (Materials science and metallurgy). Awaits wire-up.";
    return s;
}

}
