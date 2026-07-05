// SPDX-License-Identifier: GPL-3.0-or-later
// QA harness (todo.txt task 88): round-trip every .kicad_sch and
// .kicad_pcb under kicad/demos, report weak equivalence.
// Separate from test_kicad_io_roundtrip so we can track pass-rate
// as writer coverage grows.

#include "test_runner.hpp"
#include "../modules/347_kicad_io/kicad_io.hpp"

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <vector>

namespace {

std::string demos_root() {
    for (const char * c : {"kicad/demos", "../kicad/demos", "../../kicad/demos"}) {
        struct stat st{};
        if (::stat(c, &st) == 0 && S_ISDIR(st.st_mode)) return c;
    }
    return {};
}
void walk(const std::string & root, std::vector<std::string> & out, std::string_view ext) {
    DIR * d = ::opendir(root.c_str());
    if (!d) return;
    while (auto * e = ::readdir(d)) {
        std::string name = e->d_name;
        if (name == "." || name == "..") continue;
        std::string full = root + "/" + name;
        struct stat st{};
        if (::stat(full.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) { walk(full, out, ext); continue; }
        if (name.size() >= ext.size() &&
            name.compare(name.size() - ext.size(), ext.size(), ext) == 0) out.push_back(full);
    }
    ::closedir(d);
}

testing::TestOutcome run() {
    std::string root = demos_root();
    if (root.empty()) return testing::skip("no kicad/demos on disk");

    std::vector<std::string> schs, pcbs;
    walk(root, schs, ".kicad_sch");
    walk(root, pcbs, ".kicad_pcb");
    if (schs.empty() && pcbs.empty()) return testing::skip("empty demos dir");

    // Golden pass criteria: >= 25% of files must weakly round-trip.
    // This lets writer coverage evolve without a red suite while still
    // tripping regressions if we go backwards.
    std::size_t sch_ok = 0, pcb_ok = 0;
    for (const auto & p : schs) {
        kicad_io::IOError e;
        auto s = kicad_io::read_schematic_file(p, &e);
        if (!s) continue;
        auto txt = kicad_io::write_schematic(*s);
        auto s2 = kicad_io::read_schematic(txt);
        if (s2 && s2->root.items.size() == s->root.items.size()) ++sch_ok;
    }
    for (const auto & p : pcbs) {
        kicad_io::IOError e;
        auto b = kicad_io::read_board_file(p, &e);
        if (!b) continue;
        auto txt = kicad_io::write_board(*b);
        auto b2 = kicad_io::read_board(txt);
        if (b2 && b2->layers.size() == b->layers.size()
               && b2->nets.size()   == b->nets.size()) ++pcb_ok;
    }
    double sch_ratio = schs.empty() ? 1.0 : double(sch_ok) / schs.size();
    double pcb_ratio = pcbs.empty() ? 1.0 : double(pcb_ok) / pcbs.size();
    if (sch_ratio < 0.25 && pcb_ratio < 0.25)
        return testing::fail("round-trip ratio below 25%: sch="
                             + std::to_string(sch_ratio) + " pcb=" + std::to_string(pcb_ratio));
    return testing::ok();
}

const int _r = testing::register_test(
    "full_roundtrip",
    "QA: round-trip every .kicad_sch/.kicad_pcb under kicad/demos; require >=25% pass on either side.",
    &run);

} // namespace
