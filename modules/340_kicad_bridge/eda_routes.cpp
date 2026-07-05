// SPDX-License-Identifier: GPL-3.0-or-later
#include "eda_routes.hpp"

#include "kicad_bridge.hpp"
#include "drc_erc_report.hpp"
#include "../341_kicad_libs/kicad_libs.hpp"
#include "../342_kicad_project/kicad_project.hpp"
#include "../343_circuit_intent/circuit_intent.hpp"
#include "../843_schematic_capture/schematic_capture.hpp"
#include "../844_pcb_layout/pcb_layout.hpp"
#include "../849_spice_simulator/spice_simulator.hpp"
#include "../869_gerber_and_fab_output_viewer/gerber_and_fab_output_viewer.hpp"
#include "../010_interface/httplib.h"

#include <nlohmann/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace eda_routes {
namespace {

using json = nlohmann::json;

std::string mkdirp_workspace(const std::string & sess_id) {
    // sess_id is the client-supplied identifier; we build
    // /tmp/tool_eda/<sanitized>/. Every EDA op reads/writes here so
    // multiple concurrent sessions don't collide.
    std::string safe;
    safe.reserve(sess_id.size());
    for (char c : sess_id) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_') safe += c;
        else safe += '_';
    }
    if (safe.empty()) safe = "default";
    std::string base = "/tmp/tool_eda";
    ::mkdir(base.c_str(), 0755);
    std::string dir  = base + "/" + safe;
    ::mkdir(dir.c_str(), 0755);
    return dir;
}

std::string body_str(const httplib::Request & req) { return req.body; }

json json_ok(json extra = json::object()) {
    extra["ok"] = true;
    return extra;
}
json json_err(std::string_view msg) {
    return json{ {"ok", false}, {"error", std::string(msg)} };
}

void send_json(httplib::Response & res, const json & j, int status = 200) {
    res.status = status;
    res.set_content(j.dump(), "application/json");
}

std::string slurp(const std::string & path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    std::stringstream ss; ss << f.rdbuf();
    return ss.str();
}

json violation_to_json(const kicad_bridge::Violation & v) {
    return {
        {"type", v.type},
        {"severity", v.severity},
        {"description", v.description},
        {"has_pos", v.has_pos},
        {"x_mm", v.x_mm},
        {"y_mm", v.y_mm}
    };
}

json report_to_json(const kicad_bridge::Report & r) {
    json j = json::object();
    j["source"]           = r.source;
    j["kicad_version"]    = r.kicad_version;
    j["coordinate_units"] = r.coordinate_units;
    j["errors"]           = r.errors;
    j["warnings"]         = r.warnings;
    j["ignored"]          = r.ignored;
    j["summary"]          = kicad_bridge::summarize(r);
    j["violations"]       = json::array();
    for (const auto & v : r.violations)
        j["violations"].push_back(violation_to_json(v));
    j["unconnected_items"] = json::array();
    for (const auto & v : r.unconnected_items)
        j["unconnected_items"].push_back(violation_to_json(v));
    return j;
}

} // namespace

void register_all(httplib::Server & app) {
    // Idempotent init of every module we surface.
    kicad_bridge::init();
    kicad_libs::init();
    schematic_capture::init();
    pcb_layout::init();
    spice_simulator::init();
    gerber_and_fab_output_viewer::init();

    // --- Status probe ---
    app.Get("/api/eda/kicad_status", [](const httplib::Request &, httplib::Response & res) {
        const auto & c = kicad_bridge::config();
        json j = {
            {"available",       c.available},
            {"cli_path",        c.cli_path},
            {"version",         c.version},
            {"stock_data_home", c.stock_data_home},
            {"libs_ready",      kicad_libs::config().ready},
            {"symbol_libs",     kicad_libs::config().symbol_libs},
            {"footprint_libs",  kicad_libs::config().footprint_libs},
            {"spice_available", spice_simulator::available()},
            {"spice_version",   spice_simulator::version()}
        };
        send_json(res, json_ok(j));
    });

    // --- Library search ---
    app.Get("/api/eda/symbol_search", [](const httplib::Request & req, httplib::Response & res) {
        std::string q = req.get_param_value("q");
        int limit = req.has_param("limit") ? std::atoi(req.get_param_value("limit").c_str()) : 20;
        if (limit <= 0 || limit > 200) limit = 20;
        auto hits = kicad_libs::search_symbols(q, static_cast<std::size_t>(limit));
        json arr = json::array();
        for (const auto & h : hits) {
            arr.push_back({
                {"lib", h.lib},
                {"name", h.name},
                {"description", h.description},
                {"source_path", h.source_path}
            });
        }
        send_json(res, json_ok({{"hits", arr}, {"query", q}}));
    });

    app.Get("/api/eda/footprint_search", [](const httplib::Request & req, httplib::Response & res) {
        std::string q = req.get_param_value("q");
        int limit = req.has_param("limit") ? std::atoi(req.get_param_value("limit").c_str()) : 20;
        if (limit <= 0 || limit > 200) limit = 20;
        auto hits = kicad_libs::search_footprints(q, static_cast<std::size_t>(limit));
        json arr = json::array();
        for (const auto & h : hits) {
            arr.push_back({
                {"lib", h.lib},
                {"name", h.name},
                {"description", h.description},
                {"pad_count", h.pad_count},
                {"smd", h.smd},
                {"source_path", h.source_path}
            });
        }
        send_json(res, json_ok({{"hits", arr}, {"query", q}}));
    });

    // Preview SVGs generated on demand.
    app.Get("/api/eda/symbol_svg", [](const httplib::Request & req, httplib::Response & res) {
        std::string lib  = req.get_param_value("lib");
        std::string name = req.get_param_value("name");
        auto hit = kicad_libs::find_symbol(lib + ":" + name);
        if (hit.source_path.empty()) { res.status = 404; return; }
        std::string tmpl = "/tmp/tool_eda_sym_XXXXXX";
        std::vector<char> buf(tmpl.begin(), tmpl.end()); buf.push_back(0);
        int fd = ::mkstemp(buf.data());
        if (fd < 0) { res.status = 500; return; }
        ::close(fd);
        std::string out = std::string(buf.data()) + ".svg";
        ::unlink(buf.data());
        auto rr = kicad_bridge::sym_export_svg(hit.source_path, name, out);
        if (rr.exit_code != 0) { res.status = 500; res.set_content(rr.stderr_text, "text/plain"); return; }
        std::string body = slurp(out);
        ::unlink(out.c_str());
        res.set_content(body, "image/svg+xml");
    });

    // --- Circuit intent + emit ---
    app.Post("/api/eda/emit_project", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string session = body.value("session_id", std::string("default"));
        std::string title   = body.value("title",      std::string("tool_generated"));
        std::string dir     = mkdirp_workspace(session);
        auto pj = kicad_project::default_project(title);
        std::string pro_path = dir + "/" + title + ".kicad_pro";
        std::string prl_path = dir + "/" + title + ".kicad_prl";
        {
            std::ofstream f(pro_path); f << kicad_project::to_json(pj);
        }
        {
            std::ofstream f(prl_path); f << kicad_project::to_prl_json(pj);
        }
        send_json(res, json_ok({{"pro_path", pro_path}, {"prl_path", prl_path}}));
    });

    app.Post("/api/eda/emit_schematic", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string session = body.value("session_id", std::string("default"));
        std::string title   = body.value("title",      std::string("tool_generated"));

        circuit_intent::Intent intent;
        std::string err;
        std::string intent_json = body["intent"].is_string()
                                    ? body["intent"].get<std::string>()
                                    : body["intent"].dump();
        if (!circuit_intent::parse_json(intent_json, intent, err)) {
            send_json(res, json_err("intent parse failed: " + err), 400);
            return;
        }
        auto diags = circuit_intent::validate(intent);
        auto result = schematic_capture::from_intent(intent, {});
        std::string dir = mkdirp_workspace(session);
        std::string sch_path = dir + "/" + title + ".kicad_sch";
        if (!schematic_capture::write_file(sch_path, result)) {
            send_json(res, json_err("write .kicad_sch failed"), 500);
            return;
        }
        json arr = json::array();
        for (const auto & d : result.diagnostics)
            arr.push_back({{"severity", (int)d.severity}, {"field", d.field}, {"message", d.message}});
        for (const auto & d : diags)
            arr.push_back({{"severity", (int)d.severity}, {"field", d.field}, {"message", d.message}});
        send_json(res, json_ok({
            {"sch_path", sch_path},
            {"resolved_parts", result.resolved_parts},
            {"placeholder_parts", result.placeholder_parts},
            {"diagnostics", arr}
        }));
    });

    app.Post("/api/eda/emit_pcb", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string session = body.value("session_id", std::string("default"));
        std::string title   = body.value("title",      std::string("tool_generated"));
        circuit_intent::Intent intent;
        std::string err;
        std::string intent_json = body["intent"].is_string()
                                    ? body["intent"].get<std::string>()
                                    : body["intent"].dump();
        if (!circuit_intent::parse_json(intent_json, intent, err)) {
            send_json(res, json_err("intent parse failed: " + err), 400);
            return;
        }
        auto result = pcb_layout::from_intent(intent, {});
        std::string dir = mkdirp_workspace(session);
        std::string pcb_path = dir + "/" + title + ".kicad_pcb";
        if (!pcb_layout::write_file(pcb_path, result)) {
            send_json(res, json_err("write .kicad_pcb failed"), 500);
            return;
        }
        json arr = json::array();
        for (const auto & d : result.diagnostics)
            arr.push_back({{"severity", (int)d.severity}, {"field", d.field}, {"message", d.message}});
        send_json(res, json_ok({
            {"pcb_path", pcb_path},
            {"resolved_footprints", result.resolved_footprints},
            {"placeholder_footprints", result.placeholder_footprints},
            {"emitted_nets", result.emitted_nets},
            {"diagnostics", arr}
        }));
    });

    // --- ERC / DRC ---
    app.Post("/api/eda/erc", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string sch = body.value("sch_path", std::string{});
        if (sch.empty()) { send_json(res, json_err("missing sch_path"), 400); return; }
        std::string dir     = mkdirp_workspace(body.value("session_id", std::string("default")));
        std::string out     = dir + "/erc.json";
        auto rr = kicad_bridge::sch_erc(sch, out, /*json=*/true);
        if (rr.exit_code < 0) { send_json(res, json_err(rr.stderr_text), 500); return; }
        auto rep = kicad_bridge::load_report(out);
        send_json(res, json_ok({
            {"exit_code", rr.exit_code},
            {"report_path", out},
            {"report", report_to_json(rep)},
            {"stderr", rr.stderr_text}
        }));
    });

    app.Post("/api/eda/drc", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string pcb = body.value("pcb_path", std::string{});
        if (pcb.empty()) { send_json(res, json_err("missing pcb_path"), 400); return; }
        bool parity = body.value("schematic_parity", false);
        std::string dir = mkdirp_workspace(body.value("session_id", std::string("default")));
        std::string out = dir + "/drc.json";
        auto rr = kicad_bridge::pcb_drc(pcb, out, /*json=*/true, parity);
        if (rr.exit_code < 0) { send_json(res, json_err(rr.stderr_text), 500); return; }
        auto rep = kicad_bridge::load_report(out);
        send_json(res, json_ok({
            {"exit_code", rr.exit_code},
            {"report_path", out},
            {"report", report_to_json(rep)},
            {"stderr", rr.stderr_text}
        }));
    });

    // --- Netlist ---
    app.Post("/api/eda/netlist", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string sch = body.value("sch_path", std::string{});
        std::string fmt = body.value("format",   std::string("kicadsexpr"));
        if (sch.empty()) { send_json(res, json_err("missing sch_path"), 400); return; }
        std::string dir = mkdirp_workspace(body.value("session_id", std::string("default")));
        std::string out = dir + "/netlist." + (fmt == "spice" ? "cir" : "net");
        auto rr = kicad_bridge::sch_netlist(sch, out, fmt);
        if (rr.exit_code != 0) { send_json(res, json_err(rr.stderr_text), 500); return; }
        send_json(res, json_ok({{"net_path", out}}));
    });

    // --- Export ---
    app.Post("/api/eda/export", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string pcb  = body.value("pcb_path", std::string{});
        std::string kind = body.value("kind",     std::string{});
        if (pcb.empty() || kind.empty()) { send_json(res, json_err("missing pcb_path or kind"), 400); return; }
        std::string dir = mkdirp_workspace(body.value("session_id", std::string("default"))) + "/" + kind;
        ::mkdir(dir.c_str(), 0755);

        kicad_bridge::RunResult rr;
        if      (kind == "gerbers") rr = kicad_bridge::pcb_export_gerbers(pcb, dir, {});
        else if (kind == "drill")   rr = kicad_bridge::pcb_export_drill  (pcb, dir);
        else if (kind == "step")    rr = kicad_bridge::pcb_export_step   (pcb, dir + "/board.step");
        else if (kind == "svg")     rr = kicad_bridge::pcb_export_svg    (pcb, dir + "/board.svg", {});
        else if (kind == "pos")     rr = kicad_bridge::pcb_export_pos    (pcb, dir + "/pos.csv", "both", "csv");
        else { send_json(res, json_err("unknown kind: " + kind), 400); return; }

        if (rr.exit_code != 0) { send_json(res, json_err(rr.stderr_text), 500); return; }
        send_json(res, json_ok({
            {"output_path", rr.output_path},
            {"exit_code", rr.exit_code}
        }));
    });

    // --- Gerber viewer ---
    app.Post("/api/eda/render_layers", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string pcb = body.value("pcb_path", std::string{});
        if (pcb.empty()) { send_json(res, json_err("missing pcb_path"), 400); return; }
        std::string dir = mkdirp_workspace(body.value("session_id", std::string("default"))) + "/layers";
        ::mkdir(dir.c_str(), 0755);

        auto specs = gerber_and_fab_output_viewer::default_layers_2layer();
        auto rr    = gerber_and_fab_output_viewer::render(pcb, dir, specs);
        json layers = json::array();
        for (std::size_t i = 0; i < specs.size(); ++i) {
            std::string path = (i < rr.svg_paths.size()) ? rr.svg_paths[i] : std::string{};
            layers.push_back({
                {"name",       specs[i].name},
                {"display",    specs[i].display},
                {"css_color",  specs[i].css_color},
                {"default_visible", specs[i].default_visible},
                {"svg_path",   path}
            });
        }
        send_json(res, json_ok({
            {"layers", layers},
            {"combined", rr.combined_path},
            {"log", rr.log}
        }));
    });

    app.Post("/api/eda/bundle_fab", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string pcb = body.value("pcb_path", std::string{});
        if (pcb.empty()) { send_json(res, json_err("missing pcb_path"), 400); return; }
        std::string dir = mkdirp_workspace(body.value("session_id", std::string("default"))) + "/fab";
        ::mkdir(dir.c_str(), 0755);
        std::string out = gerber_and_fab_output_viewer::bundle_for_fab(pcb, dir);
        if (out.empty()) { send_json(res, json_err("bundle failed; kicad-cli unavailable"), 500); return; }
        send_json(res, json_ok({{"bundle_path", out}}));
    });

    // --- SPICE ---
    app.Post("/api/eda/spice_run", [](const httplib::Request & req, httplib::Response & res) {
        auto body = json::parse(body_str(req), nullptr, false);
        if (body.is_discarded()) { send_json(res, json_err("bad JSON"), 400); return; }
        std::string net = body.value("netlist", std::string{});
        std::string cmd = body.value("analysis", std::string{});
        if (net.empty() || cmd.empty()) { send_json(res, json_err("missing netlist or analysis"), 400); return; }
        auto rr = spice_simulator::run(net, cmd);
        json sigs = json::array();
        for (const auto & s : rr.signals) {
            sigs.push_back({
                {"name", s.name},
                {"is_complex", s.is_complex},
                {"values", s.values}
            });
        }
        send_json(res, json_ok({
            {"ok_signals", rr.ok},
            {"log", rr.log},
            {"error", rr.error},
            {"signals", sigs}
        }));
    });

    // --- Static file passthrough for anything we wrote under /tmp/tool_eda ---
    app.Get(R"(/api/eda/file)", [](const httplib::Request & req, httplib::Response & res) {
        std::string p = req.get_param_value("path");
        if (p.rfind("/tmp/tool_eda/", 0) != 0) { res.status = 403; return; }
        std::string body = slurp(p);
        if (body.empty()) { res.status = 404; return; }
        // Mime by extension.
        const char * mime = "application/octet-stream";
        if      (p.size() >= 4 && p.substr(p.size() - 4) == ".svg") mime = "image/svg+xml";
        else if (p.size() >= 5 && p.substr(p.size() - 5) == ".json") mime = "application/json";
        else if (p.size() >= 4 && p.substr(p.size() - 4) == ".pdf") mime = "application/pdf";
        else if (p.size() >= 4 && p.substr(p.size() - 4) == ".png") mime = "image/png";
        else if (p.size() >= 4 && p.substr(p.size() - 4) == ".zip") mime = "application/zip";
        res.set_content(body, mime);
    });
}

} // namespace eda_routes
