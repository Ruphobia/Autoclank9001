// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Additional fabrication-format exporters:
//   * ODB++: JobDeck / miscellaneous / steps / layers (compact
//     directory-layout formatted as a tarball-flat text bundle).
//   * HyperLynx HYP: signal-integrity input file (ASCII, hierarchical
//     keyword blocks).
//   * IDF (Intermediate Data Format) v3: mechanical CAD data exchange,
//     board outline + component placement.
//
// These are compact text formats; MVP writes a valid subset that
// downstream tools accept. Round-trip and advanced attributes are
// follow-ups.
namespace fab_export {

// Returned as a map(path -> text) representing files in the fab
// bundle. The caller can zip them, drop them into a directory, or
// stream them to the browser.
struct Bundle {
    std::unordered_map<std::string, std::string> files;
};

// ODB++ export: bundle with root/misc/attrlist, root/steps/pcb/*, etc.
Bundle write_odbpp(const kicad_model::Board & board,
                   std::string_view job_name = "ac9_board");

// HyperLynx export: single .HYP file (returned as `text`).
std::string write_hyperlynx(const kicad_model::Board & board);

// IDF v3 (.emn board + .emp component library).
struct IdfBundle {
    std::string emn;   // board data (outline + placement)
    std::string emp;   // component library (placeholder boxes)
};
IdfBundle write_idf(const kicad_model::Board & board);

} // namespace fab_export
