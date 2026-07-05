#pragma once

#include <string>

// Stub module for the "HLA typer and neoantigen pipeline" tool entry.
// Microbiology, virology, and immunology.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace hla_typer_and_neoantigen_pipeline {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
