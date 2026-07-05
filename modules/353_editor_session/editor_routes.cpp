// SPDX-License-Identifier: GPL-3.0-or-later
#include "editor_routes.hpp"

#include "editor_session.hpp"

#include "../346_kicad_model/kicad_model.hpp"
#include "../347_kicad_io/kicad_io.hpp"
#include "../349_netlist/netlist.hpp"
#include "../350_erc/erc.hpp"
#include "../351_drc/drc.hpp"
#include "../352_fab_writers/fab_writers.hpp"
#include "../354_annotator/annotator.hpp"
#include "../355_pcb_ops/pcb_ops.hpp"
#include "../356_lib_editor/lib_editor.hpp"
#include "../357_bom/bom.hpp"
#include "../358_gerber_viewer_native/gerber_viewer_native.hpp"
#include "../359_import_export/import_export.hpp"
#include "../360_zone_fill/zone_fill.hpp"
#include "../361_hierarchy/hierarchy.hpp"
#include "../362_board_setup/board_setup.hpp"
#include "../363_drill_viewer/drill_viewer.hpp"
#include "../364_pagelayout/pagelayout.hpp"
#include "../365_fast_drc/fast_drc.hpp"
#include "../366_eagle_import/eagle_import.hpp"
#include "../367_line_router/line_router.hpp"
#include "../368_ibis/ibis.hpp"
#include "../369_pns_shove/pns_shove.hpp"
#include "../370_length_tuning/length_tuning.hpp"
#include "../371_diff_pair/diff_pair.hpp"
#include "../372_step_writer/step_writer.hpp"
#include "../373_fab_export/fab_export.hpp"
#include "../374_orcad_import/orcad_import.hpp"
#include "../375_sim_model/sim_model.hpp"
#include "../376_mesh_load/mesh_load.hpp"
#include "../377_altium_import/altium_import.hpp"
#include "../378_ltspice_import/ltspice_import.hpp"
#include "../379_hit_test/hit_test.hpp"
#include "../380_ibis_timing/ibis_timing.hpp"
#include "../381_pos_overrides/pos_overrides.hpp"
#include "../010_interface/httplib.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include <sstream>
#include <string>

namespace editor_routes {
namespace {

using json = nlohmann::json;
using editor_session::store;

json ok(json extra = json::object()) { extra["ok"] = true; return extra; }
json err(std::string_view m) { return { {"ok", false}, {"error", std::string(m)} }; }
void send(httplib::Response & res, const json & j, int status = 200) {
    res.status = status;
    res.set_content(j.dump(), "application/json");
}

json body_json(const httplib::Request & r) {
    return json::parse(r.body, nullptr, /*allow_exceptions=*/false);
}

std::string session_id(const json & b) {
    if (b.is_object() && b.contains("session_id") && b["session_id"].is_string())
        return b["session_id"].get<std::string>();
    return "default";
}

json summary(const std::shared_ptr<editor_session::Session> & s) {
    json j = json::object();
    j["session_id"]   = s->id();
    j["version"]      = s->version();
    j["sch_items"]    = s->sch().root.items.size();
    j["pcb_items"]    = s->pcb().items.size();
    j["can_undo"]     = s->can_undo();
    j["can_redo"]     = s->can_redo();
    j["selection_sch"] = json::array();
    j["selection_pcb"] = json::array();
    for (const auto & u : s->selection().sch) j["selection_sch"].push_back(u);
    for (const auto & u : s->selection().pcb) j["selection_pcb"].push_back(u);
    return j;
}

// Deserialize a schematic item from a small JSON stub. Supports the
// most common item kinds; extend as tools land in the frontend.
kicad_model::ItemPtr sch_item_from_json(const json & j) {
    using namespace kicad_model;
    if (!j.is_object()) return {};
    std::string kind = j.value("kind", "");
    auto mm = [&](const char * key, int idx = -1) -> long long {
        if (!j.contains(key)) return 0;
        const auto & v = j[key];
        if (idx < 0) return geom::mm_to_nm(v.get<double>());
        return geom::mm_to_nm(v[idx].get<double>());
    };
    if (kind == "symbol") {
        auto s = std::make_shared<SchSymbol>();
        s->lib_id = j.value("lib_id", "");
        if (j.contains("at")) s->at = { mm("at", 0), mm("at", 1) };
        s->angle = geom::EDA_ANGLE{ j.value("angle", 0.0) };
        Field f_ref; f_ref.name = "Reference"; f_ref.value = j.value("reference", ""); f_ref.uuid = make_uuid();
        Field f_val; f_val.name = "Value";     f_val.value = j.value("value", "");     f_val.uuid = make_uuid();
        s->fields = { f_ref, f_val };
        return s;
    }
    if (kind == "wire") {
        auto w = std::make_shared<SchWire>();
        if (j.contains("pts")) for (auto & p : j["pts"]) w->pts.push_back({ geom::mm_to_nm(p[0].get<double>()), geom::mm_to_nm(p[1].get<double>()) });
        return w;
    }
    if (kind == "junction") {
        auto n = std::make_shared<SchJunction>();
        if (j.contains("at")) n->at = { mm("at", 0), mm("at", 1) };
        return n;
    }
    if (kind == "label" || kind == "global_label" || kind == "hier_label") {
        std::shared_ptr<SchLabel> l;
        if (kind == "global_label")   l = std::make_shared<SchGlobalLabel>();
        else if (kind == "hier_label") l = std::make_shared<SchHierLabel>();
        else                            l = std::make_shared<SchLabel>();
        l->text = j.value("text", "");
        if (j.contains("at")) l->at = { mm("at", 0), mm("at", 1) };
        l->angle = geom::EDA_ANGLE{ j.value("angle", 0.0) };
        return l;
    }
    if (kind == "no_connect") {
        auto n = std::make_shared<SchNoConnect>();
        if (j.contains("at")) n->at = { mm("at", 0), mm("at", 1) };
        return n;
    }
    return {};
}

kicad_model::ItemPtr pcb_item_from_json(const json & j) {
    using namespace kicad_model;
    if (!j.is_object()) return {};
    std::string kind = j.value("kind", "");
    auto mm = [&](const char * key, int idx = -1) -> long long {
        if (!j.contains(key)) return 0;
        const auto & v = j[key];
        if (idx < 0) return geom::mm_to_nm(v.get<double>());
        return geom::mm_to_nm(v[idx].get<double>());
    };
    if (kind == "footprint") {
        auto fp = std::make_shared<Footprint>();
        fp->lib_id = j.value("lib_id", "");
        if (j.contains("at")) fp->at = { mm("at", 0), mm("at", 1) };
        fp->angle = geom::EDA_ANGLE{ j.value("angle", 0.0) };
        fp->placement_layer = j.value("layer", "F.Cu");
        Field f_ref; f_ref.name = "Reference"; f_ref.value = j.value("reference", ""); f_ref.uuid = make_uuid();
        Field f_val; f_val.name = "Value";     f_val.value = j.value("value", "");     f_val.uuid = make_uuid();
        fp->fields = { f_ref, f_val };
        return fp;
    }
    if (kind == "track") {
        auto t = std::make_shared<PcbTrack>();
        if (j.contains("start")) t->start = { mm("start", 0), mm("start", 1) };
        if (j.contains("end"))   t->end   = { mm("end",   0), mm("end",   1) };
        t->width_nm = geom::mm_to_nm(j.value("width", 0.2));
        t->layer    = j.value("layer", "F.Cu");
        t->net      = j.value("net", 0);
        return t;
    }
    if (kind == "via") {
        auto v = std::make_shared<PcbVia>();
        if (j.contains("at")) v->at = { mm("at", 0), mm("at", 1) };
        v->size_nm  = geom::mm_to_nm(j.value("size",  0.6));
        v->drill_nm = geom::mm_to_nm(j.value("drill", 0.3));
        v->net      = j.value("net", 0);
        return v;
    }
    if (kind == "gr_line") {
        auto g = std::make_shared<GrLine>();
        if (j.contains("start")) g->start = { mm("start", 0), mm("start", 1) };
        if (j.contains("end"))   g->end   = { mm("end",   0), mm("end",   1) };
        g->layer = j.value("layer", "Edge.Cuts");
        return g;
    }
    return {};
}

} // namespace

void register_all(httplib::Server & app) {
    // ---- State snapshot ----
    app.Post("/api/eda/editor/state", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        send(res, ok(summary(s)));
    });

    app.Get("/api/eda/editor/list", [](const httplib::Request &, httplib::Response & res) {
        auto ids = store().list();
        json arr = json::array();
        for (auto & id : ids) arr.push_back(id);
        send(res, ok({{"sessions", arr}}));
    });

    // ---- Load / save ----
    app.Post("/api/eda/editor/load", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        if (!b.is_object()) { send(res, err("bad JSON"), 400); return; }
        auto s = store().get(session_id(b));
        std::string kind = b.value("kind", "");
        std::string path = b.value("path", "");
        std::string text = b.value("text", "");
        kicad_io::IOError e;
        if (kind == "sch") {
            auto model = text.empty()
                ? kicad_io::read_schematic_file(path, &e)
                : kicad_io::read_schematic(text, &e);
            if (!model) { send(res, err("read sch: " + e.message), 500); return; }
            s->set_sch(std::move(*model));
        } else if (kind == "pcb") {
            auto model = text.empty()
                ? kicad_io::read_board_file(path, &e)
                : kicad_io::read_board(text, &e);
            if (!model) { send(res, err("read pcb: " + e.message), 500); return; }
            s->set_pcb(std::move(*model));
        } else {
            send(res, err("unknown kind"), 400);
            return;
        }
        send(res, ok(summary(s)));
    });

    app.Post("/api/eda/editor/save", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        if (!b.is_object()) { send(res, err("bad JSON"), 400); return; }
        auto s = store().get(session_id(b));
        std::string kind = b.value("kind", "");
        std::string path = b.value("path", "");
        if (kind == "sch") {
            std::string txt = kicad_io::write_schematic(s->sch());
            if (path.empty())  send(res, ok({{"text", txt}}));
            else               send(res, ok({{"path", path}, {"ok_write", kicad_io::write_schematic_file(path, s->sch())}}));
        } else if (kind == "pcb") {
            std::string txt = kicad_io::write_board(s->pcb());
            if (path.empty())  send(res, ok({{"text", txt}}));
            else               send(res, ok({{"path", path}, {"ok_write", kicad_io::write_board_file(path, s->pcb())}}));
        } else send(res, err("unknown kind"), 400);
    });

    // ---- Mutations ----
    app.Post("/api/eda/editor/add", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        if (!b.is_object()) { send(res, err("bad JSON"), 400); return; }
        auto s = store().get(session_id(b));
        std::string kind = b.value("kind", "sch");
        auto item = kind == "pcb" ? pcb_item_from_json(b["item"]) : sch_item_from_json(b["item"]);
        if (!item) { send(res, err("bad item"), 400); return; }
        kicad_model::UUID id = item->uuid.empty() ? kicad_model::make_uuid() : item->uuid;
        item->uuid = id;
        bool ok_run = kind == "pcb"
            ? s->run(editor_session::add_pcb_item(s, item))
            : s->run(editor_session::add_sch_item(s, item));
        if (!ok_run) { send(res, err("add failed"), 500); return; }
        send(res, ok({{"uuid", id}, {"version", s->version()}}));
    });

    app.Post("/api/eda/editor/remove", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string kind = b.value("kind", "sch");
        std::string uuid = b.value("uuid", "");
        bool ok_run = kind == "pcb"
            ? s->run(editor_session::remove_pcb_item(s, uuid))
            : s->run(editor_session::remove_sch_item(s, uuid));
        if (!ok_run) { send(res, err("remove failed"), 500); return; }
        send(res, ok({{"version", s->version()}}));
    });

    app.Post("/api/eda/editor/move", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string kind = b.value("kind", "sch");
        std::string uuid = b.value("uuid", "");
        long long dx = geom::mm_to_nm(b.value("dx_mm", 0.0));
        long long dy = geom::mm_to_nm(b.value("dy_mm", 0.0));
        bool ok_run = kind == "pcb"
            ? s->run(editor_session::move_pcb_item(s, uuid, dx, dy))
            : s->run(editor_session::move_sch_item(s, uuid, dx, dy));
        if (!ok_run) { send(res, err("move failed"), 500); return; }
        send(res, ok({{"version", s->version()}}));
    });

    app.Post("/api/eda/editor/rotate", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string kind = b.value("kind", "sch");
        std::string uuid = b.value("uuid", "");
        double deg = b.value("deg", 90.0);
        bool ok_run = kind == "pcb"
            ? s->run(editor_session::rotate_pcb_item(s, uuid, deg))
            : s->run(editor_session::rotate_sch_item(s, uuid, deg));
        if (!ok_run) { send(res, err("rotate failed"), 500); return; }
        send(res, ok({{"version", s->version()}}));
    });

    app.Post("/api/eda/editor/edit_field", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string uuid  = b.value("uuid", "");
        std::string field = b.value("field", "");
        std::string val   = b.value("value", "");
        bool ok_run = s->run(editor_session::edit_sch_field(s, uuid, field, val));
        if (!ok_run) { send(res, err("edit_field failed"), 500); return; }
        send(res, ok({{"version", s->version()}}));
    });

    app.Post("/api/eda/editor/undo", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        send(res, ok({{"undone", s->undo()}, {"version", s->version()}}));
    });
    app.Post("/api/eda/editor/redo", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        send(res, ok({{"redone", s->redo()}, {"version", s->version()}}));
    });

    // ---- Selection ----
    app.Post("/api/eda/editor/selection", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        s->selection().clear();
        if (b.contains("sch") && b["sch"].is_array())
            for (auto & u : b["sch"]) s->selection().sch.insert(u.get<std::string>());
        if (b.contains("pcb") && b["pcb"].is_array())
            for (auto & u : b["pcb"]) s->selection().pcb.insert(u.get<std::string>());
        send(res, ok({{"selected", s->selection().size()}}));
    });

    // ---- Refdes annotate ----
    app.Post("/api/eda/editor/annotate", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        annotator::Options opts;
        opts.start_at         = b.value("start_at", 1);
        opts.loose_unannotated= b.value("loose", true);
        auto rep = annotator::annotate(s->sch(), opts);
        json changes = json::array();
        for (const auto & c : rep.changes)
            changes.push_back({{"uuid", c.uuid}, {"old", c.old_ref}, {"new", c.new_ref}});
        send(res, ok({{"changes", changes}, {"version", s->version()}}));
    });

    // ---- Copy / paste (session-local clipboard) ----
    app.Post("/api/eda/editor/copy_selection", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        // Serialize each selected schematic item to JSON pairs
        // ("uuid", inline sch fragment) so paste can re-emit it. MVP:
        // for schematic side only; PCB copy is a follow-up.
        json copies = json::array();
        for (const auto & u : s->selection().sch) {
            auto it = s->find_sch(u);
            if (!it) continue;
            // For MVP, dump the item as a mini JSON just enough for paste
            // to reconstruct. Only handles SchSymbol at this pass.
            if (it->type == kicad_model::ItemType::SchSymbol) {
                auto * sym = static_cast<kicad_model::SchSymbol*>(it.get());
                copies.push_back({
                    {"kind","symbol"},
                    {"lib_id", sym->lib_id},
                    {"at_mm", { geom::nm_to_mm(sym->at.x), geom::nm_to_mm(sym->at.y) }},
                    {"angle", sym->angle.deg()},
                    {"reference", sym->reference()},
                    {"value", sym->value()}
                });
            }
        }
        s->clipboard_put("application/x-eda-selection", copies.dump());
        send(res, ok({{"count", copies.size()}}));
    });

    app.Post("/api/eda/editor/paste", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        double dx_mm = b.value("dx_mm", 5.0);
        double dy_mm = b.value("dy_mm", 5.0);
        auto clip = s->clipboard_get();
        if (clip.first != "application/x-eda-selection") {
            send(res, err("clipboard empty"), 400); return;
        }
        json arr = json::parse(clip.second, nullptr, false);
        if (!arr.is_array()) { send(res, err("clipboard corrupt"), 500); return; }
        int placed = 0;
        for (const auto & j : arr) {
            if (!j.is_object() || j.value("kind","") != "symbol") continue;
            auto sym = std::make_shared<kicad_model::SchSymbol>();
            sym->lib_id = j.value("lib_id","");
            double x = j["at_mm"][0].get<double>() + dx_mm;
            double y = j["at_mm"][1].get<double>() + dy_mm;
            sym->at = { geom::mm_to_nm(x), geom::mm_to_nm(y) };
            sym->angle = geom::EDA_ANGLE{ j.value("angle", 0.0) };
            kicad_model::Field f_ref; f_ref.name = "Reference"; f_ref.value = j.value("reference","?");
            f_ref.at = sym->at; f_ref.uuid = kicad_model::make_uuid();
            kicad_model::Field f_val; f_val.name = "Value";     f_val.value = j.value("value","");
            f_val.at = sym->at; f_val.uuid = kicad_model::make_uuid();
            sym->fields = { f_ref, f_val };
            if (s->run(editor_session::add_sch_item(s, sym))) ++placed;
        }
        send(res, ok({{"placed", placed}, {"version", s->version()}}));
    });

    // ---- On-demand analysis using the session's live model ----
    app.Post("/api/eda/editor/run_erc", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        auto nl  = netlist::derive(s->sch());
        auto rep = erc::run(s->sch(), nl, {});
        send(res, ok({
            {"errors",   rep.errors},
            {"warnings", rep.warnings},
            {"json",     erc::to_kicad_json(rep, "session:" + s->id())}
        }));
    });
    app.Post("/api/eda/editor/run_drc", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        auto rep = drc::run(s->pcb(), {});
        send(res, ok({
            {"errors",   rep.errors},
            {"warnings", rep.warnings},
            {"json",     drc::to_kicad_json(rep, "session:" + s->id())}
        }));
    });
    // ---- Teardrops ----
    app.Post("/api/eda/editor/teardrops", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        pcb_ops::TeardropOptions opts;
        opts.length_ratio_of_width = b.value("length_ratio", 3.0);
        opts.width_ratio_of_track  = b.value("width_ratio",  1.2);
        opts.include_vias = b.value("include_vias", true);
        opts.include_pads = b.value("include_pads", true);
        auto n = pcb_ops::generate_teardrops(s->pcb(), opts);
        send(res, ok({{"count", n}, {"version", s->version()}}));
    });

    // ---- Alignment / distribution ----
    app.Post("/api/eda/editor/align", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string axis = b.value("axis", "center_x");
        std::vector<std::string> uuids;
        if (b.contains("uuids") && b["uuids"].is_array())
            for (auto & u : b["uuids"]) uuids.push_back(u.get<std::string>());
        pcb_ops::AlignAxis a = pcb_ops::AlignAxis::CenterX;
        if      (axis == "left")     a = pcb_ops::AlignAxis::LeftX;
        else if (axis == "right")    a = pcb_ops::AlignAxis::RightX;
        else if (axis == "top")      a = pcb_ops::AlignAxis::TopY;
        else if (axis == "bottom")   a = pcb_ops::AlignAxis::BottomY;
        else if (axis == "center_x") a = pcb_ops::AlignAxis::CenterX;
        else if (axis == "center_y") a = pcb_ops::AlignAxis::CenterY;
        pcb_ops::align(s->pcb(), uuids, a);
        send(res, ok({{"version", s->version()}}));
    });
    app.Post("/api/eda/editor/distribute", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        bool horizontal = b.value("horizontal", true);
        std::vector<std::string> uuids;
        if (b.contains("uuids") && b["uuids"].is_array())
            for (auto & u : b["uuids"]) uuids.push_back(u.get<std::string>());
        pcb_ops::distribute(s->pcb(), uuids, horizontal);
        send(res, ok({{"version", s->version()}}));
    });

    // ---- BOM ----
    app.Post("/api/eda/editor/bom", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        bom::Options opts;
        if (b.contains("columns") && b["columns"].is_array())
            for (auto & c : b["columns"]) opts.columns.push_back(c.get<std::string>());
        opts.ref_combine          = b.value("combine",          std::string("list"));
        opts.exclude_dnp          = b.value("exclude_dnp",      false);
        opts.exclude_power_symbols= b.value("exclude_power",    true);
        auto out = bom::generate(s->sch(), opts);
        std::string fmt = b.value("format", "json");
        if      (fmt == "csv")  send(res, ok({{"format","csv"},  {"text", bom::to_csv(out)}}));
        else if (fmt == "html") send(res, ok({{"format","html"}, {"text", bom::to_html(out)}}));
        else                    send(res, ok({{"format","json"}, {"text", bom::to_json(out)}}));
    });

    // ---- Server-side hit-test ----
    app.Post("/api/eda/editor/pick", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string kind = b.value("kind", std::string("sch"));
        double x = b.value("x_mm", 0.0), y = b.value("y_mm", 0.0);
        double radius = b.value("radius_mm", 1.0);
        auto hits = (kind == "pcb")
            ? hit_test::pick_pcb(s->pcb(), x, y, radius)
            : hit_test::pick_sch(s->sch(), x, y, radius);
        json arr = json::array();
        for (const auto & h : hits) arr.push_back({
            {"uuid", h.uuid}, {"kind", h.kind},
            {"bbox", {h.bbox_mm[0], h.bbox_mm[1], h.bbox_mm[2], h.bbox_mm[3]}}
        });
        send(res, ok({{"hits", arr}}));
    });
    app.Post("/api/eda/editor/box_select", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string kind = b.value("kind", std::string("sch"));
        double lo_x = b["lo"][0].get<double>(), lo_y = b["lo"][1].get<double>();
        double hi_x = b["hi"][0].get<double>(), hi_y = b["hi"][1].get<double>();
        auto hits = (kind == "pcb")
            ? hit_test::select_pcb(s->pcb(), lo_x, lo_y, hi_x, hi_y)
            : hit_test::select_sch(s->sch(), lo_x, lo_y, hi_x, hi_y);
        json arr = json::array();
        for (const auto & h : hits) arr.push_back({{"uuid", h.uuid}, {"kind", h.kind}});
        send(res, ok({{"hits", arr}}));
    });

    // ---- IBIS timing metrics ----
    app.Post("/api/eda/editor/ibis_timing", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string text = b.value("text", "");
        std::string model = b.value("model", "");
        auto f = ibis::parse(text);
        if (!f) { send(res, err("parse failed"), 500); return; }
        auto m = ibis_timing::evaluate(*f, model);
        send(res, ok({
            {"model", m.model_name},
            {"vinh",  m.vinh},
            {"vinl",  m.vinl},
            {"c_comp_pF", m.c_comp_pF},
            {"drive_impedance_ohm", m.drive_impedance_ohm},
            {"rise_time_ns", m.rise_time_ns},
            {"fall_time_ns", m.fall_time_ns},
            {"warnings", m.warnings}
        }));
    });

    // ---- Pos overrides ----
    app.Post("/api/eda/editor/pos_overrides", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string overrides_json = b.value("overrides", std::string("{}"));
        auto t = pos_overrides::from_json(overrides_json);
        std::string csv = pos_overrides::write_pos_csv(s->pcb(), t);
        send(res, ok({{"csv", csv}}));
    });

    // ---- 3D mesh load ----
    app.Post("/api/eda/editor/mesh_load", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string text = b.value("text", "");
        std::string ext  = b.value("ext",  "");
        auto m = mesh_load::load(text, ext);
        if (!m) { send(res, err("unsupported or empty mesh"), 400); return; }
        res.status = 200;
        res.set_content(mesh_load::to_json(*m), "application/json");
    });

    // ---- Altium ASCII PCB import ----
    app.Post("/api/eda/editor/import_altium_ascii", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string text = b.value("text", "");
        altium_import::ImportReport rep;
        auto board = altium_import::read_board(text, &rep);
        if (!board || !rep.ok) { send(res, err(rep.warnings.empty() ? std::string("no PCB header") : rep.warnings), 500); return; }
        s->set_pcb(std::move(*board));
        send(res, ok({
            {"tracks", rep.tracks}, {"vias", rep.vias},
            {"components", rep.components}, {"fills", rep.fills},
            {"version", s->version()}
        }));
    });

    // ---- LTspice .asc import ----
    app.Post("/api/eda/editor/import_ltspice", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string text = b.value("text", "");
        auto f = ltspice_import::parse(text);
        if (!f) { send(res, err("parse failed"), 500); return; }
        json syms = json::array();
        for (const auto & s : f->symbols)
            syms.push_back({{"type",s.type},{"x",s.x},{"y",s.y},{"rot",s.rotation},{"ref",s.inst_name},{"value",s.value}});
        json wires = json::array();
        for (const auto & w : f->wires)
            wires.push_back({{"x1",w.x1},{"y1",w.y1},{"x2",w.x2},{"y2",w.y2}});
        json flags = json::array();
        for (const auto & fl : f->flags)
            flags.push_back({{"x",fl.x},{"y",fl.y},{"name",fl.name}});
        send(res, ok({{"symbols", syms}, {"wires", wires}, {"flags", flags}}));
    });

    // ---- STEP export ----
    app.Post("/api/eda/editor/step_export", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        step_writer::Options opts;
        opts.substrate_thickness_mm = b.value("thickness_mm", 1.6);
        std::string txt = step_writer::write(s->pcb(), opts);
        send(res, ok({{"format", "step"}, {"text", txt}}));
    });

    // ---- ODB++ / HyperLynx / IDF ----
    app.Post("/api/eda/editor/odbpp", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string job = b.value("job_name", std::string("ac9_board"));
        auto bundle = fab_export::write_odbpp(s->pcb(), job);
        json files = json::object();
        for (const auto & kv : bundle.files) files[kv.first] = kv.second;
        send(res, ok({{"format", "odbpp"}, {"files", files}}));
    });
    app.Post("/api/eda/editor/hyperlynx", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        send(res, ok({{"format","hyp"}, {"text", fab_export::write_hyperlynx(s->pcb())}}));
    });
    app.Post("/api/eda/editor/idf", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        auto idf = fab_export::write_idf(s->pcb());
        send(res, ok({{"format","idf"}, {"emn", idf.emn}, {"emp", idf.emp}}));
    });

    // ---- OrCAD legacy netlist import ----
    app.Post("/api/eda/editor/import_orcad", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string text = b.value("text", "");
        auto f = orcad_import::parse(text);
        if (!f) { send(res, err("parse failed"), 500); return; }
        json comps = json::array();
        for (const auto & c : f->components)
            comps.push_back({{"ref",c.ref},{"package",c.package},{"value",c.value}});
        json nets = json::array();
        for (const auto & n : f->nets) {
            json pins = json::array();
            for (const auto & p : n.pins) pins.push_back({{"ref",p.ref},{"pin",p.pin}});
            nets.push_back({{"name",n.name},{"pins",pins}});
        }
        send(res, ok({{"components", comps}, {"nets", nets}}));
    });

    // ---- SIM_MODEL editor ----
    app.Get("/api/eda/editor/sim_model_catalog", [](const httplib::Request &, httplib::Response & res) {
        json arr = json::array();
        for (const auto & m : sim_model::catalog()) arr.push_back(json::parse(sim_model::to_json(m), nullptr, false));
        send(res, ok({{"models", arr}}));
    });
    app.Post("/api/eda/editor/sim_model_emit", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string json_text = b.value("model", std::string("{}"));
        sim_model::Model m = sim_model::from_json(json_text);
        send(res, ok({{"spice", sim_model::to_spice(m)}}));
    });

    // ---- PNS shove ----
    app.Post("/api/eda/editor/pns_add_track", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        kicad_model::PcbTrack pt;
        pt.start = { geom::mm_to_nm(b["start"][0].get<double>()), geom::mm_to_nm(b["start"][1].get<double>()) };
        pt.end   = { geom::mm_to_nm(b["end"][0].get<double>()),   geom::mm_to_nm(b["end"][1].get<double>()) };
        pt.width_nm = geom::mm_to_nm(b.value("width_mm", 0.2));
        pt.layer    = b.value("layer", std::string("F.Cu"));
        pt.net      = b.value("net", 0);
        auto rr = pns_shove::add_track(s->pcb(), pt, {});
        json shoves = json::array();
        for (const auto & sh : rr.applied_shoves)
            shoves.push_back({{"uuid", sh.uuid},
                              {"dx_mm", geom::nm_to_mm(sh.dx_nm)},
                              {"dy_mm", geom::nm_to_mm(sh.dy_nm)}});
        if (!rr.ok) { send(res, err(rr.reason), 500); return; }
        send(res, ok({{"applied_shoves", shoves}, {"version", s->version()}}));
    });
    app.Post("/api/eda/editor/pns_drag_track", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string uuid = b.value("uuid", "");
        long long dx = geom::mm_to_nm(b.value("dx_mm", 0.0));
        long long dy = geom::mm_to_nm(b.value("dy_mm", 0.0));
        auto rr = pns_shove::drag_track(s->pcb(), uuid, dx, dy, {});
        if (!rr.ok) { send(res, err(rr.reason), 500); return; }
        send(res, ok({{"applied_shoves", rr.applied_shoves.size()}, {"version", s->version()}}));
    });

    // ---- Length tuning ----
    app.Post("/api/eda/editor/length_tune", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        length_tuning::MeanderOptions opts;
        opts.target_length_mm = b.value("target_mm",    0.0);
        opts.amplitude_mm     = b.value("amplitude_mm", 1.5);
        opts.period_mm        = b.value("period_mm",    3.0);
        std::string uuid = b.value("uuid", "");
        auto rr = length_tuning::apply(s->pcb(), uuid, opts);
        if (!rr.ok) { send(res, err(rr.reason), 500); return; }
        send(res, ok({
            {"segments_added", rr.segments_added},
            {"achieved_mm",    rr.achieved_length_mm},
            {"version",        s->version()}
        }));
    });

    // ---- Differential pair ----
    app.Post("/api/eda/editor/diff_pair", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        diff_pair::Config cfg;
        cfg.track_width_nm = geom::mm_to_nm(b.value("width_mm", 0.2));
        cfg.gap_nm         = geom::mm_to_nm(b.value("gap_mm",   0.25));
        cfg.layer          = b.value("layer", std::string("F.Cu"));
        cfg.p_net          = b.value("p_net", 0);
        cfg.n_net          = b.value("n_net", 0);
        geom::VECTOR2I ps  = { geom::mm_to_nm(b["p_start"][0].get<double>()), geom::mm_to_nm(b["p_start"][1].get<double>()) };
        geom::VECTOR2I pe  = { geom::mm_to_nm(b["p_end"][0].get<double>()),   geom::mm_to_nm(b["p_end"][1].get<double>()) };
        geom::VECTOR2I ns  = { geom::mm_to_nm(b["n_start"][0].get<double>()), geom::mm_to_nm(b["n_start"][1].get<double>()) };
        geom::VECTOR2I ne  = { geom::mm_to_nm(b["n_end"][0].get<double>()),   geom::mm_to_nm(b["n_end"][1].get<double>()) };
        auto rr = diff_pair::route(s->pcb(), ps, pe, ns, ne, cfg);
        if (!rr.ok) { send(res, err(rr.reason), 500); return; }
        send(res, ok({
            {"p_segments", rr.p_segments},
            {"n_segments", rr.n_segments},
            {"version",    s->version()}
        }));
    });

    // ---- Fast DRC (live feedback for track / via drawing) ----
    app.Post("/api/eda/editor/fast_drc_track", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        fast_drc::ProposedTrack pt;
        pt.start   = { geom::mm_to_nm(b["start"][0].get<double>()),
                       geom::mm_to_nm(b["start"][1].get<double>()) };
        pt.end     = { geom::mm_to_nm(b["end"][0].get<double>()),
                       geom::mm_to_nm(b["end"][1].get<double>()) };
        pt.width_nm = geom::mm_to_nm(b.value("width_mm", 0.2));
        pt.layer   = b.value("layer", std::string("F.Cu"));
        pt.net     = b.value("net", 0);
        auto hits = fast_drc::check_track(s->pcb(), pt, {});
        json arr = json::array();
        for (const auto & h : hits) arr.push_back({
            {"other_uuid", h.other_uuid}, {"reason", h.reason},
            {"clearance_mm", h.clearance_mm},
            {"at", {geom::nm_to_mm(h.at.x), geom::nm_to_mm(h.at.y)}}
        });
        send(res, ok({{"collisions", arr}}));
    });
    app.Post("/api/eda/editor/fast_drc_via", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        fast_drc::ProposedVia pv;
        pv.at       = { geom::mm_to_nm(b["at"][0].get<double>()),
                        geom::mm_to_nm(b["at"][1].get<double>()) };
        pv.size_nm  = geom::mm_to_nm(b.value("size_mm",  0.6));
        pv.drill_nm = geom::mm_to_nm(b.value("drill_mm", 0.3));
        pv.net      = b.value("net", 0);
        auto hits = fast_drc::check_via(s->pcb(), pv, {});
        json arr = json::array();
        for (const auto & h : hits) arr.push_back({
            {"other_uuid", h.other_uuid}, {"reason", h.reason},
            {"clearance_mm", h.clearance_mm},
            {"at", {geom::nm_to_mm(h.at.x), geom::nm_to_mm(h.at.y)}}
        });
        send(res, ok({{"collisions", arr}}));
    });

    // ---- EAGLE .brd import ----
    app.Post("/api/eda/editor/import_eagle_brd", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string text = b.value("text", "");
        eagle_import::ImportReport rep;
        auto board = eagle_import::read_board(text, &rep);
        if (!board) { send(res, err(rep.warnings), 500); return; }
        s->set_pcb(std::move(*board));
        send(res, ok({
            {"elements", rep.elements}, {"signals", rep.signals},
            {"wires", rep.wires}, {"vias", rep.vias},
            {"edge_cuts", rep.edge_cuts}, {"version", s->version()}
        }));
    });

    // ---- Simple line router ----
    app.Post("/api/eda/editor/line_route", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        line_router::Config cfg;
        cfg.clearance_mm    = b.value("clearance_mm",    0.15);
        cfg.track_width_nm  = geom::mm_to_nm(b.value("track_width_mm", 0.2));
        cfg.layer           = b.value("layer",           std::string("F.Cu"));
        auto rep = line_router::route_all_unrouted(s->pcb(), cfg);
        send(res, ok({
            {"routed_pairs",     rep.routed_pairs},
            {"obstructed_pairs", rep.obstructed_pairs},
            {"log",              rep.log},
            {"version",          s->version()}
        }));
    });

    // ---- IBIS parse (returns model info; no persistent binding yet) ----
    app.Post("/api/eda/editor/parse_ibis", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string text = b.value("text", "");
        auto p = ibis::parse(text);
        if (!p) { send(res, err("parse failed"), 500); return; }
        json out = json::object();
        out["ibis_ver"]     = p->header.ibis_ver;
        out["component"]    = p->header.component;
        out["manufacturer"] = p->header.manufacturer;
        json pins = json::array();
        for (const auto & pin : p->pins)
            pins.push_back({{"name",pin.name},{"signal",pin.signal_name},{"model",pin.model_name}});
        out["pins"] = pins;
        json models = json::array();
        for (const auto & m : p->models) models.push_back({
            {"name", m.name}, {"model_type", m.model_type}, {"c_comp_pF", m.c_comp_pF},
            {"iv_tables", m.iv.size()}, {"vt_tables", m.vt.size()}
        });
        out["models"] = models;
        send(res, ok(out));
    });

    // ---- Board setup ----
    app.Post("/api/eda/editor/board_setup", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        auto st = board_setup::extract(s->pcb());
        send(res, ok({{"text", board_setup::to_json(st)}}));
    });
    app.Put("/api/eda/editor/board_setup", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string text = b.value("text", "");
        auto st = board_setup::from_json(text);
        board_setup::apply(s->pcb(), st);
        send(res, ok({{"version", s->version()}}));
    });

    // ---- Native drill viewer ----
    app.Post("/api/eda/editor/drill_svg", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string text = b.value("text", "");
        if (text.empty()) { send(res, err("missing text"), 400); return; }
        res.status = 200;
        res.set_content(drill_viewer::render_to_svg(text, {}), "image/svg+xml");
    });

    // ---- Pagelayout (.kicad_wks) ----
    app.Post("/api/eda/editor/pagelayout", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string text = b.value("text", "");
        auto ds = text.empty()
                    ? pagelayout::default_a4_titleblock()
                    : pagelayout::read(text);
        json out = {
            {"page_width_mm",   ds.page_width_mm},
            {"page_height_mm",  ds.page_height_mm},
            {"lines",           json::array()},
            {"rects",           json::array()},
            {"texts",           json::array()}
        };
        for (const auto & L : ds.lines) out["lines"].push_back({
            {"start", {geom::nm_to_mm(L.start.x), geom::nm_to_mm(L.start.y)}},
            {"end",   {geom::nm_to_mm(L.end.x),   geom::nm_to_mm(L.end.y)}},
            {"width", L.width_mm}
        });
        for (const auto & R : ds.rects) out["rects"].push_back({
            {"start", {geom::nm_to_mm(R.start.x), geom::nm_to_mm(R.start.y)}},
            {"end",   {geom::nm_to_mm(R.end.x),   geom::nm_to_mm(R.end.y)}},
            {"width", R.width_mm}
        });
        for (const auto & T : ds.texts) out["texts"].push_back({
            {"text",    T.text},
            {"pos",     {geom::nm_to_mm(T.at.x), geom::nm_to_mm(T.at.y)}},
            {"font",    {T.font_h_mm, T.font_v_mm}},
            {"bold",    T.bold},
            {"italic",  T.italic},
            {"justify", T.justify},
            {"name",    T.name}
        });
        send(res, ok(out));
    });

    // ---- PCB copy/paste (mirror of the schematic side) ----
    app.Post("/api/eda/editor/copy_selection_pcb", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        json copies = json::array();
        for (const auto & u : s->selection().pcb) {
            auto it = s->find_pcb(u);
            if (!it) continue;
            if (it->type == kicad_model::ItemType::PcbFootprint) {
                auto * fp = static_cast<kicad_model::Footprint*>(it.get());
                std::string ref, val;
                for (const auto & f : fp->fields) {
                    if (f.name == "Reference") ref = f.value;
                    if (f.name == "Value")     val = f.value;
                }
                copies.push_back({
                    {"kind",      "footprint"},
                    {"lib_id",    fp->lib_id},
                    {"at_mm",     {geom::nm_to_mm(fp->at.x), geom::nm_to_mm(fp->at.y)}},
                    {"angle",     fp->angle.deg()},
                    {"layer",     fp->placement_layer},
                    {"reference", ref},
                    {"value",     val}
                });
            }
        }
        s->clipboard_put("application/x-eda-selection-pcb", copies.dump());
        send(res, ok({{"count", copies.size()}}));
    });
    app.Post("/api/eda/editor/paste_pcb", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        double dx_mm = b.value("dx_mm", 5.0);
        double dy_mm = b.value("dy_mm", 5.0);
        auto clip = s->clipboard_get();
        if (clip.first != "application/x-eda-selection-pcb") { send(res, err("clipboard empty"), 400); return; }
        json arr = json::parse(clip.second, nullptr, false);
        if (!arr.is_array()) { send(res, err("clipboard corrupt"), 500); return; }
        int placed = 0;
        for (const auto & j : arr) {
            if (!j.is_object() || j.value("kind","") != "footprint") continue;
            auto fp = std::make_shared<kicad_model::Footprint>();
            fp->lib_id          = j.value("lib_id", "");
            fp->placement_layer = j.value("layer", "F.Cu");
            fp->at              = { geom::mm_to_nm(j["at_mm"][0].get<double>() + dx_mm),
                                     geom::mm_to_nm(j["at_mm"][1].get<double>() + dy_mm) };
            fp->angle           = geom::EDA_ANGLE{ j.value("angle", 0.0) };
            fp->uuid            = kicad_model::make_uuid();
            kicad_model::Field f_ref; f_ref.name  = "Reference"; f_ref.value = j.value("reference","?"); f_ref.uuid = kicad_model::make_uuid();
            kicad_model::Field f_val; f_val.name  = "Value";     f_val.value = j.value("value","");    f_val.uuid = kicad_model::make_uuid();
            fp->fields = { f_ref, f_val };
            if (s->run(editor_session::add_pcb_item(s, fp))) ++placed;
        }
        send(res, ok({{"placed", placed}, {"version", s->version()}}));
    });

    // ---- Mirror + Flip ----
    app.Post("/api/eda/editor/mirror", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string uuid = b.value("uuid", "");
        std::string ax   = b.value("axis", "x");
        bool ok_run = s->run(editor_session::mirror_sch_item(s, uuid, ax.empty() ? 'x' : ax[0]));
        if (!ok_run) { send(res, err("mirror failed"), 500); return; }
        send(res, ok({{"version", s->version()}}));
    });
    app.Post("/api/eda/editor/flip", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string uuid = b.value("uuid", "");
        bool ok_run = s->run(editor_session::flip_pcb_item(s, uuid));
        if (!ok_run) { send(res, err("flip failed"), 500); return; }
        send(res, ok({{"version", s->version()}}));
    });

    // ---- Zone fill ----
    app.Post("/api/eda/editor/zone_fill", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        zone_fill::Options opts;
        opts.extra_clearance_mm = b.value("extra_clearance_mm", 0.0);
        auto r_ = zone_fill::fill_all(s->pcb(), opts);
        send(res, ok({
            {"zones", r_.zones_processed},
            {"obstacles", r_.obstacles_carved},
            {"version", s->version()}
        }));
    });

    // ---- Hierarchy load / navigate ----
    app.Post("/api/eda/editor/load_hierarchy", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        hierarchy::LoadOptions opts;
        opts.max_depth = b.value("max_depth", 8);
        opts.base_dir  = b.value("base_dir",  std::string{});
        auto r_ = hierarchy::load_children(s->sch(), opts);
        json warns = json::array();
        for (const auto & w : r_.warnings) warns.push_back(w);
        send(res, ok({
            {"sheets_loaded", r_.sheets_loaded},
            {"warnings",      warns},
            {"version",       s->version()}
        }));
    });

    // ---- Specctra DSN / SES + IPC-2581 ----
    app.Post("/api/eda/editor/dsn", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string txt = import_export::to_dsn(s->pcb(), {});
        send(res, ok({{"format", "dsn"}, {"text", txt}}));
    });
    app.Post("/api/eda/editor/apply_ses", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string ses = b.value("text", "");
        auto r_ = import_export::apply_ses(s->pcb(), ses);
        send(res, ok({
            {"new_tracks", r_.new_tracks},
            {"new_vias",   r_.new_vias},
            {"warnings",   r_.warnings},
            {"version",    s->version()}
        }));
    });
    app.Post("/api/eda/editor/ipc2581", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string txt = import_export::to_ipc2581(s->pcb());
        send(res, ok({{"format", "ipc2581"}, {"text", txt}}));
    });

    // ---- Native gerber viewer (parse .gbr -> SVG) ----
    app.Post("/api/eda/editor/gerber_svg", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string text = b.value("text", "");
        if (text.empty()) { send(res, err("missing text"), 400); return; }
        std::string svg = gerber_viewer_native::render_to_svg(text, {});
        res.status = 200;
        res.set_content(svg, "image/svg+xml");
    });

    // ---- Library editing: commit a lib_symbol into the session ----
    app.Post("/api/eda/editor/lib_symbol_save", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string lib_id = b.value("lib_id", "");
        if (lib_id.empty()) { send(res, err("missing lib_id"), 400); return; }
        kicad_model::LibSymbol L;
        L.lib_id = lib_id;
        if (b.contains("fields") && b["fields"].is_array()) {
            for (auto & f : b["fields"]) {
                kicad_model::Field F; F.name = f.value("name", ""); F.value = f.value("value", "");
                F.uuid = kicad_model::make_uuid();
                L.fields.push_back(F);
            }
        }
        if (b.contains("pins") && b["pins"].is_array()) {
            for (auto & p : b["pins"]) {
                kicad_model::SchPin P;
                P.number     = p.value("number", "");
                P.name       = p.value("name", "~");
                P.electrical = p.value("electrical", "passive");
                if (p.contains("at_mm") && p["at_mm"].is_array()) {
                    P.at = { geom::mm_to_nm(p["at_mm"][0].get<double>()),
                             geom::mm_to_nm(p["at_mm"][1].get<double>()) };
                }
                P.length_mm = p.value("length_mm", 2.54);
                P.uuid = kicad_model::make_uuid();
                L.pins.push_back(P);
            }
        }
        s->sch().lib_symbols[lib_id] = std::move(L);
        send(res, ok({{"lib_id", lib_id}, {"version", s->version()}}));
    });

    app.Post("/api/eda/editor/footprint_save", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string lib_id = b.value("lib_id", "");
        if (lib_id.empty()) { send(res, err("missing lib_id"), 400); return; }
        auto fp = std::make_shared<kicad_model::Footprint>();
        fp->lib_id = lib_id;
        fp->uuid = kicad_model::make_uuid();
        if (b.contains("pads") && b["pads"].is_array()) {
            for (auto & p : b["pads"]) {
                kicad_model::Pad P;
                P.number = p.value("number", "");
                P.kind   = p.value("kind",   "smd");
                P.shape  = p.value("shape",  "rect");
                if (p.contains("at_mm"))   P.at   = { geom::mm_to_nm(p["at_mm"][0].get<double>()),
                                                       geom::mm_to_nm(p["at_mm"][1].get<double>()) };
                if (p.contains("size_mm")) P.size = { geom::mm_to_nm(p["size_mm"][0].get<double>()),
                                                       geom::mm_to_nm(p["size_mm"][1].get<double>()) };
                if (p.contains("layers") && p["layers"].is_array())
                    for (auto & L : p["layers"]) P.layers.push_back(L.get<std::string>());
                P.uuid = kicad_model::make_uuid();
                fp->pads.push_back(P);
            }
        }
        // MVP: keep the crafted footprint in a session-scratch bag so we
        // can round-trip it out. For now, add it to the Board as an item
        // so the "add footprint" tool can reference lib_id on next place.
        s->pcb().items.push_back(fp);
        send(res, ok({{"lib_id", lib_id}, {"uuid", fp->uuid}, {"version", s->version()}}));
    });

    // ---- 3D model summary probe ----
    app.Post("/api/eda/editor/mesh_summary", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        std::string ext  = b.value("ext",  "");
        std::string text = b.value("text", "");
        auto ms = lib_editor::summarize_mesh(text, ext);
        send(res, ok({
            {"format",   ms.format},
            {"vertices", ms.vertices},
            {"faces",    ms.faces},
            {"lo",       {ms.lo_x, ms.lo_y, ms.lo_z}},
            {"hi",       {ms.hi_x, ms.hi_y, ms.hi_z}}
        }));
    });

    // ---- Cross-probe ----
    app.Post("/api/eda/editor/cross_probe", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string ref = b.value("ref", "");
        json out = json::object();
        out["sch"] = pcb_ops::sch_uuids_for_ref(s->sch(), ref);
        out["pcb"] = pcb_ops::pcb_uuids_for_ref(s->pcb(), ref);
        send(res, ok(out));
    });

    app.Post("/api/eda/editor/derive_netlist", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s   = store().get(session_id(b));
        auto nl  = netlist::derive(s->sch());
        std::string fmt = b.value("format", "kicadsexpr");
        std::string txt = fmt == "spice"
            ? netlist::to_spice_netlist(nl, s->sch(), b.value("analysis", ""))
            : netlist::to_kicad_netlist(nl, s->sch());
        send(res, ok({{"format", fmt}, {"text", txt}}));
    });
    app.Post("/api/eda/editor/write_fab", [](const httplib::Request & req, httplib::Response & res) {
        json b = body_json(req);
        auto s = store().get(session_id(b));
        std::string layer = b.value("layer", "F.Cu");
        std::string kind  = b.value("kind",  "gerber");
        if (kind == "gerber") {
            std::string g = fab_writers::write_gerber_layer(s->pcb(), layer, {});
            send(res, ok({{"layer", layer}, {"text", g}}));
        } else if (kind == "drill_pth") {
            send(res, ok({{"text", fab_writers::write_drill_pth(s->pcb(), {})}}));
        } else if (kind == "drill_npth") {
            send(res, ok({{"text", fab_writers::write_drill_npth(s->pcb(), {})}}));
        } else if (kind == "pos") {
            send(res, ok({{"text", fab_writers::write_pos_csv(s->pcb())}}));
        } else send(res, err("unknown kind"), 400);
    });

    // ---- SSE stream of version bumps (task 24 seed) ----
    // Client subscribes; server holds the connection open and sends
    // `data: {"version":N}\n\n` whenever the session's version changes.
    // MVP: 500 ms polling loop, terminates when version has not
    // advanced in 60 seconds (client should reconnect).
    app.Get("/api/eda/editor/events", [](const httplib::Request & req, httplib::Response & res) {
        std::string sid = req.get_param_value("session_id");
        if (sid.empty()) sid = "default";
        auto s = store().get(sid);
        res.set_chunked_content_provider("text/event-stream",
            [s](std::size_t, httplib::DataSink & sink) {
                static thread_local std::uint64_t last = 0;
                if (last == 0) last = s->version();
                std::uint64_t now = s->version();
                if (now != last) {
                    last = now;
                    std::string line = "data: {\"version\":" + std::to_string(now) + "}\n\n";
                    sink.write(line.data(), line.size());
                }
                return true;
            });
    });
}

} // namespace editor_routes
