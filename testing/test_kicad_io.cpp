// SPDX-License-Identifier: GPL-3.0-or-later
// Round-trip smoke test for modules/347_kicad_io.

#include "test_runner.hpp"
#include "../modules/347_kicad_io/kicad_io.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

namespace {

std::string slurp(const char * p) {
    std::ifstream f{p, std::ios::binary};
    if (!f) return {};
    std::stringstream ss; ss << f.rdbuf();
    return ss.str();
}

testing::TestOutcome run() {
    // Handcrafted mini schematic: parse, mutate, emit, reparse.
    const char * kMini =
        "(kicad_sch\n"
        "  (version 20250114)\n"
        "  (generator \"tool_test\")\n"
        "  (generator_version \"9.0\")\n"
        "  (uuid \"11111111-2222-3333-4444-555555555555\")\n"
        "  (paper \"A4\")\n"
        "  (lib_symbols\n"
        "    (symbol \"Device:R\"\n"
        "      (property \"Reference\" \"R\" (at 0 0 0) (effects (font (size 1.27 1.27))))\n"
        "      (property \"Value\"     \"R\" (at 0 0 0) (effects (font (size 1.27 1.27))))\n"
        "      (pin passive line (at 0 2.54 270) (length 2.54) (name \"~\" ) (number \"1\"))\n"
        "      (pin passive line (at 0 -2.54 90) (length 2.54) (name \"~\" ) (number \"2\"))\n"
        "    )\n"
        "  )\n"
        "  (symbol\n"
        "    (lib_id \"Device:R\")\n"
        "    (at 50 50 0)\n"
        "    (unit 1)\n"
        "    (in_bom yes) (on_board yes) (dnp no)\n"
        "    (uuid \"aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee\")\n"
        "    (property \"Reference\" \"R1\" (at 52 47 0) (effects (font (size 1.27 1.27))))\n"
        "    (property \"Value\"     \"10k\" (at 52 53 0) (effects (font (size 1.27 1.27))))\n"
        "  )\n"
        "  (wire (pts (xy 50 50) (xy 60 50)) (stroke (width 0) (type default)) (uuid \"cccccccc-cccc-cccc-cccc-cccccccccccc\"))\n"
        "  (junction (at 60 50) (diameter 0) (uuid \"dddddddd-dddd-dddd-dddd-dddddddddddd\"))\n"
        "  (global_label \"+5V\" (shape input) (at 60 50 0) (uuid \"eeeeeeee-eeee-eeee-eeee-eeeeeeeeeeee\"))\n"
        "  (sheet_instances (path \"/\" (page \"1\")))\n"
        "  (embedded_fonts no)\n"
        ")\n";

    kicad_io::IOError err;
    auto sch = kicad_io::read_schematic(kMini, &err);
    if (!sch) return testing::fail("read schematic: " + err.message);

    if (sch->version != "20250114") return testing::fail("version");
    if (sch->lib_symbols.count("Device:R") != 1) return testing::fail("lib_symbols missing R");
    if (sch->root.items.size() < 4)   return testing::fail("items count");

    // First item is the SchSymbol.
    auto * sym = sch->root.items[0]->as<kicad_model::SchSymbol>();
    if (!sym) return testing::fail("first item not symbol");
    if (sym->lib_id != "Device:R") return testing::fail("symbol lib_id");
    if (sym->reference() != "R1")  return testing::fail("symbol ref");
    if (sym->value()     != "10k") return testing::fail("symbol value");

    // Write and re-read.
    std::string emitted = kicad_io::write_schematic(*sch);
    if (emitted.find("(kicad_sch") == std::string::npos) return testing::fail("no header emitted");
    auto sch2 = kicad_io::read_schematic(emitted, &err);
    if (!sch2) return testing::fail("re-read after write: " + err.message);
    if (sch2->root.items.size() != sch->root.items.size()) return testing::fail("item count changed on round-trip");

    // If the vendored microwave demo is on disk, exercise the PCB path.
    const char * pcb_path = ::access("kicad/demos/microwave/microwave.kicad_pcb", R_OK) == 0
        ? "kicad/demos/microwave/microwave.kicad_pcb"
        : (::access("../kicad/demos/microwave/microwave.kicad_pcb", R_OK) == 0
            ? "../kicad/demos/microwave/microwave.kicad_pcb"
            : nullptr);
    if (pcb_path) {
        auto board = kicad_io::read_board_file(pcb_path, &err);
        if (!board) return testing::fail("read microwave board: " + err.message);
        if (board->layers.empty()) return testing::fail("no layers parsed");
        if (board->nets.size() < 1) return testing::fail("no nets parsed");
        // Just verify we produced *something* on write; strict round-trip
        // is task 10 (round-trip harness).
        std::string s = kicad_io::write_board(*board);
        if (s.find("(kicad_pcb") == std::string::npos) return testing::fail("wrote no kicad_pcb");
    }

    return testing::ok();
}

const int _r = testing::register_test(
    "kicad_io",
    "Read a handcrafted .kicad_sch, verify model, round-trip through writer. If microwave demo present, also read/write its .kicad_pcb.",
    &run);

} // namespace
