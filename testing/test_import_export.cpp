// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/359_import_export/import_export.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, "");
    intern_net(b, "+5V");
    intern_net(b, "GND");

    auto edge = std::make_shared<GrLine>();
    edge->start = {0, 0}; edge->end = {geom::mm_to_nm(50), 0};
    edge->layer = "Edge.Cuts"; edge->uuid = make_uuid();
    b.items.push_back(edge);

    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:R"; fp->at = {geom::mm_to_nm(10), geom::mm_to_nm(10)};
    fp->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    { Pad p; p.number="1"; p.shape="circle"; p.at={0,0};
      p.size = {geom::mm_to_nm(1.5), geom::mm_to_nm(1.5)}; p.layers={"F.Cu","F.Mask"};
      p.net=1; p.net_name="+5V"; fp->pads.push_back(p);
    }
    fp->uuid = make_uuid();
    b.items.push_back(fp);

    // DSN
    std::string dsn = import_export::to_dsn(b);
    if (dsn.find("(pcb") == std::string::npos)        return testing::fail("dsn header");
    if (dsn.find("R1")   == std::string::npos)        return testing::fail("dsn placement");
    if (dsn.find("+5V")  == std::string::npos)        return testing::fail("dsn net");

    // SES import (synthetic)
    const char * ses =
        "(session smoke\n"
        "  (routes (network_out\n"
        "    (net +5V (wire (path F.Cu 0.2 10 10 25 10)) (via Round 20 10))\n"
        "  ))\n"
        ")\n";
    auto res = import_export::apply_ses(b, ses);
    if (!res.ok || res.new_tracks < 1 || res.new_vias < 1)
        return testing::fail("ses import: tracks=" + std::to_string(res.new_tracks) +
                              " vias=" + std::to_string(res.new_vias) +
                              " msg=" + res.warnings);

    // IPC-2581
    std::string ipc = import_export::to_ipc2581(b);
    if (ipc.find("IPC-2581") == std::string::npos) return testing::fail("ipc header");
    if (ipc.find("R1")       == std::string::npos) return testing::fail("ipc component");

    return testing::ok();
}

const int _r = testing::register_test(
    "import_export",
    "Specctra DSN write, SES import (adds tracks + vias), IPC-2581 skeleton.",
    &run);

} // namespace
