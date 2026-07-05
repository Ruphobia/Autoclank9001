// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>
#include <vector>

// SPICE simulator, backed by libngspice loaded at runtime via dlopen so
// tool can build even when the dev package isn't installed.
//
// Netlist-in, waveforms-out. Uses ngspice's shared-library API
// (ngSpice_Init, ngSpice_Command, ngGet_Vec_Info, ngSpice_AllVecs).
//
// See scratchpad/kicad_integration.md §12.
namespace spice_simulator {

struct Signal {
    std::string name;              // "time", "V(out)", "I(R1)"
    std::vector<double> values;    // real part (imaginary discarded for now)
    bool                is_complex = false;
};

struct RunResult {
    std::vector<Signal> signals;
    std::string         log;       // stdout+stderr from ngspice
    bool                ok = false;
    std::string         error;     // populated when ok == false
};

bool available();
std::string version();

void init();
void shutdown();

struct Status {
    bool        ready = false;
    std::string detail;
};
Status status();

RunResult run(std::string_view netlist,
              std::string_view analysis_command);

RunResult run_file(std::string_view netlist_path,
                   std::string_view analysis_command);

std::string sample_rc_netlist();

}
