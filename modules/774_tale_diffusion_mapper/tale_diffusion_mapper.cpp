#include "tale_diffusion_mapper.hpp"

namespace tale_diffusion_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tale Diffusion Mapper (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
