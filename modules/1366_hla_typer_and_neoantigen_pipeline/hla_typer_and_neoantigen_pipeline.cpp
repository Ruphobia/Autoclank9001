#include "hla_typer_and_neoantigen_pipeline.hpp"

namespace hla_typer_and_neoantigen_pipeline {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HLA typer and neoantigen pipeline (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
