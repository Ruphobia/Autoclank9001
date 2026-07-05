// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/361_hierarchy/hierarchy.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;

    // Path helpers.
    hierarchy::Path p;
    if (!p.at_root())              return testing::fail("root default");
    p.enter("aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee");
    if (p.at_root())               return testing::fail("still root after enter");
    if (p.display() != "/aaaaaaaa/") return testing::fail("display: " + p.display());
    p.leave();
    if (!p.at_root())              return testing::fail("root after leave");

    // Load without files: sheets referencing missing paths produce warnings but no crash.
    Schematic sch; sch.uuid = make_uuid();
    auto sheet = std::make_shared<SchSheet>();
    sheet->uuid = make_uuid();
    sheet->fields.push_back({"Sheetname","child",{},{},false,false,false,1,1,{},make_uuid()});
    sheet->fields.push_back({"Sheetfile","child.kicad_sch",{},{},false,false,false,1,1,{},make_uuid()});
    sch.root.items.push_back(sheet);

    hierarchy::LoadOptions opts; opts.base_dir = "/tmp/does-not-exist";
    auto rep = hierarchy::load_children(sch, opts);
    if (rep.sheets_loaded != 0)    return testing::fail("nothing should be loaded");
    if (rep.warnings.empty())      return testing::fail("expected a missing-file warning");

    // Manually populate child_screens and re-check screen_at.
    SchScreen child;
    auto txt = std::make_shared<SchText>();
    txt->text = "hello"; txt->at = {0, 0};
    child.items.push_back(txt);
    sch.child_screens[sheet->uuid] = child;

    hierarchy::Path pp; pp.enter(sheet->uuid);
    auto * scr = hierarchy::screen_at(sch, pp);
    if (!scr || scr->items.size() != 1) return testing::fail("screen_at did not resolve child");

    // Flatten includes both root sheet + child sheet's items.
    auto flat = hierarchy::flatten(sch);
    if (flat.size() < 2) return testing::fail("flatten did not include child items");

    return testing::ok();
}

const int _r = testing::register_test(
    "hierarchy",
    "Hierarchical schematics: Path navigation, load_children on missing files, flatten includes child items.",
    &run);

} // namespace
