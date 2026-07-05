// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/366_eagle_import/eagle_import.hpp"

namespace {

testing::TestOutcome run() {
    const char * kBrd = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="7.7.0">
  <drawing>
    <settings>
      <setting alwaysvectorfont="no"/>
      <setting verticaltext="up"/>
    </settings>
    <grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="yes" altdistance="0.01" altunitdist="inch" altunit="inch"/>
    <layers>
      <layer number="1" name="Top" color="4" fill="1" visible="yes" active="yes"/>
      <layer number="16" name="Bottom" color="1" fill="1" visible="yes" active="yes"/>
      <layer number="20" name="Dimension" color="15" fill="1" visible="yes" active="yes"/>
    </layers>
    <board>
      <plain>
        <wire x1="0"  y1="0"  x2="50" y2="0"  width="0.1" layer="20"/>
        <wire x1="50" y1="0"  x2="50" y2="30" width="0.1" layer="20"/>
        <wire x1="50" y1="30" x2="0"  y2="30" width="0.1" layer="20"/>
        <wire x1="0"  y1="30" x2="0"  y2="0"  width="0.1" layer="20"/>
      </plain>
      <libraries/>
      <attributes/>
      <variantdefs/>
      <classes><class number="0" name="default" width="0" drill="0"/></classes>
      <designrules/>
      <autorouter/>
      <elements>
        <element name="R1" library="rcl" package="0603" value="10k" x="10" y="10" rot="R0"/>
        <element name="C1" library="rcl" package="0603" value="10nF" x="20" y="10" rot="R90"/>
      </elements>
      <signals>
        <signal name="N$1">
          <wire x1="10" y1="10" x2="20" y2="10" width="0.2" layer="1"/>
          <via x="15" y="10" drill="0.3" diameter="0.6"/>
        </signal>
        <signal name="GND">
          <wire x1="20" y1="10" x2="20" y2="20" width="0.3" layer="16"/>
        </signal>
      </signals>
    </board>
  </drawing>
</eagle>
)";

    eagle_import::ImportReport rep;
    auto b = eagle_import::read_board(kBrd, &rep);
    if (!b) return testing::fail("read_board: " + rep.warnings);
    if (rep.edge_cuts < 4) return testing::fail("expected 4 edge cuts, got " + std::to_string(rep.edge_cuts));
    if (rep.elements  != 2) return testing::fail("expected 2 elements, got " + std::to_string(rep.elements));
    if (rep.signals   != 2) return testing::fail("expected 2 signals, got " + std::to_string(rep.signals));
    if (rep.wires     != 2) return testing::fail("expected 2 wires, got " + std::to_string(rep.wires));
    if (rep.vias      != 1) return testing::fail("expected 1 via, got " + std::to_string(rep.vias));

    // Verify one footprint and one track landed in the model.
    bool saw_r1 = false, saw_track_top = false;
    for (const auto & it : b->items) {
        if (it->type == kicad_model::ItemType::PcbFootprint) {
            auto * fp = static_cast<kicad_model::Footprint*>(it.get());
            for (const auto & f : fp->fields) if (f.name == "Reference" && f.value == "R1") saw_r1 = true;
        }
        if (it->type == kicad_model::ItemType::PcbTrack) {
            auto * t = static_cast<kicad_model::PcbTrack*>(it.get());
            if (t->layer == "F.Cu") saw_track_top = true;
        }
    }
    if (!saw_r1)        return testing::fail("R1 not landed");
    if (!saw_track_top) return testing::fail("no F.Cu track in model");
    return testing::ok();
}

const int _r = testing::register_test(
    "eagle_import",
    "EAGLE .brd XML: plain wires -> Edge.Cuts, elements -> footprints, signals -> nets + tracks + vias.",
    &run);

} // namespace
