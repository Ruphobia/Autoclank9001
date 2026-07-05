// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/364_pagelayout/pagelayout.hpp"

namespace {

testing::TestOutcome run() {
    auto ds = pagelayout::default_a4_titleblock();
    if (ds.rects.size() < 2) return testing::fail("expected border + titleblock rects");
    if (ds.texts.empty())    return testing::fail("expected text lines");

    std::string s = pagelayout::write(ds);
    if (s.find("(kicad_wks") == std::string::npos) return testing::fail("wks header");
    if (s.find("(tbtext")    == std::string::npos) return testing::fail("no tbtext lines");

    auto ds2 = pagelayout::read(s);
    if (ds2.texts.size() != ds.texts.size()) return testing::fail("round-trip text count");
    if (ds2.rects.size() != ds.rects.size()) return testing::fail("round-trip rect count");

    std::string expanded = pagelayout::expand("Title: ${TITLE}, Rev: ${REV}",
        {{"TITLE","Blinky"}, {"REV","1"}});
    if (expanded != "Title: Blinky, Rev: 1") return testing::fail("expand");

    return testing::ok();
}

const int _r = testing::register_test(
    "pagelayout",
    "Drawing sheet: read/write .kicad_wks, ${VAR} expansion, default A4 title block.",
    &run);

} // namespace
