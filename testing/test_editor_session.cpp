// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for modules/353_editor_session.

#include "test_runner.hpp"
#include "../modules/346_kicad_model/kicad_model.hpp"
#include "../modules/353_editor_session/editor_session.hpp"

#include <memory>

namespace {

testing::TestOutcome run() {
    using namespace kicad_model;
    auto sess = editor_session::store().get("test-session");
    if (!sess) return testing::fail("no session");

    // Add a symbol.
    auto sym = std::make_shared<SchSymbol>();
    sym->lib_id = "Device:R";
    sym->at     = {geom::mm_to_nm(50), geom::mm_to_nm(50)};
    sym->fields.push_back({"Reference","R1",{},{},false,false,false,1,1,{},make_uuid()});
    sym->uuid   = make_uuid();
    UUID id     = sym->uuid;

    if (!sess->run(editor_session::add_sch_item(sess, sym)))
        return testing::fail("add failed");
    if (sess->sch().root.items.size() != 1) return testing::fail("add did not add");

    // Move it.
    if (!sess->run(editor_session::move_sch_item(sess, id, geom::mm_to_nm(10), 0)))
        return testing::fail("move failed");
    if (sess->find_sch(id)->as<SchSymbol>()->at.x != geom::mm_to_nm(60))
        return testing::fail("move did not update x");

    // Rotate.
    if (!sess->run(editor_session::rotate_sch_item(sess, id, 90)))
        return testing::fail("rotate failed");
    if (sess->find_sch(id)->as<SchSymbol>()->angle.deg() != 90)
        return testing::fail("rotate did not update angle");

    // Edit field.
    if (!sess->run(editor_session::edit_sch_field(sess, id, "Value", "1k")))
        return testing::fail("edit field failed");
    if (sess->find_sch(id)->as<SchSymbol>()->value() != "1k")
        return testing::fail("value did not update");

    // Undo all four.
    if (!sess->undo()) return testing::fail("undo 1 (edit)");
    if (sess->find_sch(id)->as<SchSymbol>()->value() == "1k")
        return testing::fail("edit not undone");
    if (!sess->undo()) return testing::fail("undo 2 (rotate)");
    if (sess->find_sch(id)->as<SchSymbol>()->angle.deg() != 0)
        return testing::fail("rotate not undone");
    if (!sess->undo()) return testing::fail("undo 3 (move)");
    if (sess->find_sch(id)->as<SchSymbol>()->at.x != geom::mm_to_nm(50))
        return testing::fail("move not undone");
    if (!sess->undo()) return testing::fail("undo 4 (add)");
    if (sess->sch().root.items.size() != 0) return testing::fail("add not undone");

    // Redo one.
    if (!sess->redo()) return testing::fail("redo 1 (add)");
    if (sess->sch().root.items.size() != 1) return testing::fail("redo did not restore");

    // Clean up.
    editor_session::store().drop("test-session");
    return testing::ok();
}

const int _r = testing::register_test(
    "editor_session",
    "Server-side editor: add/move/rotate/edit-field commands with full undo/redo stack.",
    &run);

} // namespace
