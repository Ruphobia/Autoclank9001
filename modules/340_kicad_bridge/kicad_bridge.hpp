// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>
#include <vector>

// Subprocess wrapper around `kicad-cli`. The MVP integration path from
// scratchpad/kicad_integration.md: tool synthesizes .kicad_sch / .kicad_pcb
// and drives KiCad's headless CLI for ERC, DRC, netlist, gerbers, drill,
// step, svg, pos, pdf, and the 3D PNG render. In-process kiface loading
// is a later optimization once the subprocess path is real.
namespace kicad_bridge {

struct RunResult {
    int         exit_code = -1;
    std::string stdout_text;
    std::string stderr_text;
    std::string output_path;   // set when the operation produces a single file
};

// Resolved once at init() and cached. `available` false means kicad-cli
// could not be located; every RunResult from a helper will return exit
// code -1 with an explanatory stderr_text and callers should not fail
// tool itself just because KiCad isn't installed.
struct Config {
    std::string cli_path;              // absolute path to kicad-cli, e.g. /usr/bin/kicad-cli
    std::string stock_data_home;       // KICAD_STOCK_DATA_HOME target (or empty)
    std::string version;               // as reported by kicad-cli version
    bool        available = false;
};

void init();
void shutdown();

const Config & config();

// Low-level: run `<cli_path> <args...>` with an optional cwd and env
// overlay. Exposed for callers that need commands not yet wrapped below,
// and for the smoke test. `input` is written to stdin if non-empty.
RunResult run(const std::vector<std::string> & args,
              std::string_view cwd = {},
              std::string_view input = {});

// --- Schematic ------------------------------------------------------

RunResult sch_erc(std::string_view sch_path,
                  std::string_view report_out,
                  bool json_format = true);

RunResult sch_netlist(std::string_view sch_path,
                      std::string_view net_out,
                      std::string_view format = "kicadsexpr");

RunResult sch_export_svg(std::string_view sch_path,
                         std::string_view dir_out);

RunResult sch_export_pdf(std::string_view sch_path,
                         std::string_view pdf_out);

RunResult sch_export_bom(std::string_view sch_path,
                         std::string_view csv_out,
                         std::string_view fields);

// --- PCB ------------------------------------------------------------

RunResult pcb_drc(std::string_view pcb_path,
                  std::string_view report_out,
                  bool json_format = true,
                  bool schematic_parity = true);

RunResult pcb_export_gerbers(std::string_view pcb_path,
                             std::string_view dir_out,
                             const std::vector<std::string> & layers);

RunResult pcb_export_drill(std::string_view pcb_path,
                           std::string_view dir_out);

RunResult pcb_export_step(std::string_view pcb_path,
                          std::string_view step_out);

RunResult pcb_export_svg(std::string_view pcb_path,
                         std::string_view svg_out,
                         const std::vector<std::string> & layers);

RunResult pcb_export_pos(std::string_view pcb_path,
                         std::string_view csv_out,
                         std::string_view side = "both",
                         std::string_view fmt  = "csv");

RunResult pcb_render_png(std::string_view pcb_path,
                         std::string_view png_out,
                         int width_px  = 1280,
                         int height_px = 720,
                         std::string_view side = "top");

// --- Footprints / Symbols -------------------------------------------

RunResult fp_export_svg(std::string_view lib_path,
                        std::string_view footprint_name,
                        std::string_view svg_out);

RunResult sym_export_svg(std::string_view lib_path,
                         std::string_view symbol_name,
                         std::string_view svg_out);

}
