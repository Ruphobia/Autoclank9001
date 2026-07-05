// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/340_kicad_bridge.

#include "test_runner.hpp"
#include "../modules/340_kicad_bridge/kicad_bridge.hpp"

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>
#include <unistd.h>

namespace {

// Locate a demo .kicad_pcb we can DRC without KiCad's stock data. The
// vendored microwave demo is small and self-contained.
std::string microwave_demo_pcb() {
    const char * candidates[] = {
        "kicad/demos/microwave/microwave.kicad_pcb",
        "../kicad/demos/microwave/microwave.kicad_pcb",
    };
    for (const char * c : candidates) {
        if (::access(c, R_OK) == 0) return c;
    }
    return {};
}

testing::TestOutcome run() {
    kicad_bridge::init();

    const auto & cfg = kicad_bridge::config();
    if (!cfg.available) {
        return testing::skip("kicad-cli not installed; set TOOL_KICAD_CLI or build KiCad");
    }
    if (cfg.version.empty()) {
        return testing::fail("kicad-cli reported no version");
    }

    // Round-trip through the low-level runner so we prove the fork/exec
    // capture path works and returns the same version string.
    auto r = kicad_bridge::run({"version"});
    if (r.exit_code != 0) {
        return testing::fail("`kicad-cli version` failed: exit=" +
                             std::to_string(r.exit_code) +
                             " stderr=" + r.stderr_text);
    }
    if (r.stdout_text.find(cfg.version) == std::string::npos) {
        return testing::fail("version mismatch: cache=" + cfg.version +
                             " live=" + r.stdout_text);
    }

    // If the vendored microwave demo is on disk, exercise pcb_drc against
    // it. DRC on that board is not required to be violation-free; we only
    // assert the exit code is well-defined (0 = clean, nonzero = violations
    // present, but the subprocess itself did run to completion) and a
    // report file was written.
    std::string pcb = microwave_demo_pcb();
    if (!pcb.empty()) {
        char tmpl[] = "/tmp/tool_kicad_drc_XXXXXX";
        int fd = ::mkstemp(tmpl);
        if (fd < 0) return testing::fail("mkstemp: failed");
        ::close(fd);
        std::string out = tmpl;

        auto drc = kicad_bridge::pcb_drc(pcb, out, /*json=*/true,
                                         /*schematic_parity=*/false);
        if (drc.exit_code < 0) {
            ::unlink(out.c_str());
            return testing::fail("pcb_drc dispatch failed: " + drc.stderr_text);
        }
        std::ifstream f(out);
        bool has_report = f.good() && f.peek() != std::ifstream::traits_type::eof();
        ::unlink(out.c_str());
        if (!has_report) {
            return testing::fail("pcb_drc produced no report; exit=" +
                                 std::to_string(drc.exit_code) +
                                 " stderr=" + drc.stderr_text);
        }
    }

    kicad_bridge::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "kicad_bridge",
    "Discover kicad-cli, echo its version, DRC the vendored microwave demo.",
    &run);

} // namespace
