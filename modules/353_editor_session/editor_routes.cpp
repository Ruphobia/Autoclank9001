// SPDX-License-Identifier: GPL-3.0-or-later
#include "editor_routes.hpp"

#include "editor_session.hpp"

#include "../346_kicad_model/kicad_model.hpp"
#include "../347_kicad_io/kicad_io.hpp"
#include "../349_netlist/netlist.hpp"
#include "../350_erc/erc.hpp"
#include "../351_drc/drc.hpp"
#include "../352_fab_writers/fab_writers.hpp"
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
