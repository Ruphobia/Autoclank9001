// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/373_fab_export/fab_export.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    Board b; b.uuid = make_uuid(); b.layers = default_2layer_stackup();
    intern_net(b, ""); intern_net(b, "N1");
    for (int i = 0; i < 2; ++i) {
        auto e = std::make_shared<GrLine>();
        e->start = { geom::mm_to_nm(0), geom::mm_to_nm(i * 30) };
        e->end   = { geom::mm_to_nm(50), geom::mm_to_nm(i * 30) };
        e->layer = "Edge.Cuts"; e->uuid = make_uuid();
        b.items.push_back(e);
    }
    auto fp = std::make_shared<Footprint>();
    fp->lib_id = "test:R";
    fp->at = { geom::mm_to_nm(20), geom::mm_to_nm(10) };
    fp->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    fp->uuid = make_uuid();
    b.items.push_back(fp);

    // ODB++
    auto ob = fab_export::write_odbpp(b, "job");
    if (ob.files.count("job/misc/info")            == 0) return testing::fail("odb info");
    if (ob.files.count("job/matrix/matrix")        == 0) return testing::fail("odb matrix");
    if (ob.files.count("job/steps/pcb/stephdr")    == 0) return testing::fail("odb stephdr");
    if (ob.files["job/steps/pcb/eda/data"].find("N1") == std::string::npos) return testing::fail("odb net");

    // HyperLynx
    std::string hyp = fab_export::write_hyperlynx(b);
    if (hyp.find("{VERSION")   == std::string::npos) return testing::fail("hyp header");
    if (hyp.find("{STACKUP")   == std::string::npos) return testing::fail("hyp stackup");
    if (hyp.find("{DEVICES")   == std::string::npos) return testing::fail("hyp devices");

    // IDF
    auto idf = fab_export::write_idf(b);
    if (idf.emn.find(".BOARD_OUTLINE") == std::string::npos) return testing::fail("idf outline");
    if (idf.emn.find(".PLACEMENT")     == std::string::npos) return testing::fail("idf placement");
    if (idf.emp.find(".ELECTRICAL")    == std::string::npos) return testing::fail("idf electrical");

    return testing::ok();
}

const int _r = testing::register_test(
    "fab_export",
    "ODB++ / HyperLynx HYP / IDF v3 exporters produce recognizable envelopes.",
    &run);

} // namespace
