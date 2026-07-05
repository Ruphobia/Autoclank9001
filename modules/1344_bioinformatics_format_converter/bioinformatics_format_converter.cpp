#include "bioinformatics_format_converter.hpp"

namespace bioinformatics_format_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bioinformatics Format Converter (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
