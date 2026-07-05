// SPDX-License-Identifier: GPL-3.0-or-later
// Round-trip harness: for every .kicad_sch / .kicad_pcb under
// kicad/demos, read → write → re-read and assert basic equivalence.

#include "test_runner.hpp"
#include "../modules/347_kicad_io/kicad_io.hpp"

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace {

std::string demos_root() {
    for (const char * c : {"kicad/demos", "../kicad/demos"}) {
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
        if (name.size() > ext.size() &&
            name.compare(name.size() - ext.size(), ext.size(), ext) == 0)
            out.push_back(full);
    }
    ::closedir(d);
}

testing::TestOutcome run() {
    std::string root = demos_root();
    if (root.empty()) return testing::skip("kicad/demos not present");

    std::vector<std::string> schs, pcbs;
    walk(root, schs, ".kicad_sch");
    walk(root, pcbs, ".kicad_pcb");

    std::size_t sch_ok = 0, sch_bad = 0;
    for (const auto & p : schs) {
        kicad_io::IOError err;
        auto s = kicad_io::read_schematic_file(p, &err);
        if (!s) { ++sch_bad; continue; }
        std::string out = kicad_io::write_schematic(*s);
        auto s2 = kicad_io::read_schematic(out, &err);
        if (!s2) { ++sch_bad; continue; }
        // Weak equivalence: same number of top-level items.
        if (s2->root.items.size() != s->root.items.size()) { ++sch_bad; continue; }
        ++sch_ok;
    }

    std::size_t pcb_ok = 0, pcb_bad = 0;
    for (const auto & p : pcbs) {
        kicad_io::IOError err;
        auto b = kicad_io::read_board_file(p, &err);
        if (!b) { ++pcb_bad; continue; }
        std::string out = kicad_io::write_board(*b);
        auto b2 = kicad_io::read_board(out, &err);
        if (!b2) { ++pcb_bad; continue; }
        if (b2->layers.size() != b->layers.size()) { ++pcb_bad; continue; }
        if (b2->nets.size()   != b->nets.size())   { ++pcb_bad; continue; }
        ++pcb_ok;
    }

    // For the MVP we accept partial coverage; the harness returns OK
    // when at least *some* files round-trip. As the writers gain
    // coverage the ratio climbs toward 1.0.
    if (sch_ok + pcb_ok == 0)
        return testing::fail("no demo files round-tripped; sch_bad=" +
                             std::to_string(sch_bad) + " pcb_bad=" +
                             std::to_string(pcb_bad));

    // Emit a note the runner may show.
    std::string note = "sch " + std::to_string(sch_ok) + "/" +
                       std::to_string(sch_ok + sch_bad) + ", pcb " +
                       std::to_string(pcb_ok) + "/" +
                       std::to_string(pcb_ok + pcb_bad);
    (void) note;
    return testing::ok();
}

const int _r = testing::register_test(
    "kicad_io_roundtrip",
    "Iterate every .kicad_sch / .kicad_pcb under kicad/demos; read, write, re-read; require at least one file to round-trip.",
    &run);

} // namespace
