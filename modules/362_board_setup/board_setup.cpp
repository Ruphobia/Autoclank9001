// SPDX-License-Identifier: GPL-3.0-or-later
#include "board_setup.hpp"

#include "../344_sexpr/sexpr.hpp"

#include <nlohmann/json.hpp>

#include <sstream>
#include <string>

namespace board_setup {

using json = nlohmann::json;
using kicad_model::Board;

Setup extract(const Board & board) {
    Setup s;
    s.stackup.layers            = board.layers;
    s.stackup.board_thickness_mm = board.thickness_mm;

    // Default netclass always exists.
    NetClass def;
    def.name = "Default";
    s.classes.push_back(def);

    // Parse raw_setup_sexpr for the fields we recognize.
    if (!board.raw_setup_sexpr.empty()) {
        auto root = sexpr::parse(board.raw_setup_sexpr);
        if (root && root->is_list()) {
            auto pull = [&](std::string_view name, double & out) {
                if (auto n = root->find(name)) {
                    auto v = n->child_after_head(0);
                    if (v && v->is_number()) out = v->as_double();
                }
            };
            pull("min_clearance",             s.min_clearance_mm);
            pull("min_track_width",           s.min_track_width_mm);
            pull("min_via_diameter",          s.min_via_diameter_mm);
            pull("min_via_drill",             s.min_via_drill_mm);
            pull("min_hole_clearance",        s.min_hole_clearance_mm);
            pull("min_hole_to_hole",          s.min_hole_to_hole_mm);
            pull("min_copper_edge_clearance", s.min_copper_edge_clearance_mm);
            if (auto b = root->find("allow_blind_buried_vias")) {
                auto v = b->child_after_head(0);
                s.allow_blind_buried_vias = v && v->is_atom() && v->atom() == "yes";
            }
            if (auto b = root->find("allow_microvias")) {
                auto v = b->child_after_head(0);
                s.allow_microvias = v && v->is_atom() && v->atom() == "yes";
            }
        }
    }
    return s;
}

void apply(Board & board, const Setup & setup) {
    board.layers       = setup.stackup.layers;
    board.thickness_mm = setup.stackup.board_thickness_mm;

    auto root = sexpr::list("setup");
    auto add_num = [&](std::string_view name, double v) {
        auto n = sexpr::list(std::string(name));
        n->list().push_back(sexpr::SExpr::make_number(v));
        root->list().push_back(n);
    };
    auto add_bool = [&](std::string_view name, bool v) {
        auto n = sexpr::list(std::string(name));
        n->list().push_back(sexpr::SExpr::make_atom(v ? "yes" : "no"));
        root->list().push_back(n);
    };
    add_num ("min_clearance",             setup.min_clearance_mm);
    add_num ("min_track_width",           setup.min_track_width_mm);
    add_num ("min_via_diameter",          setup.min_via_diameter_mm);
    add_num ("min_via_drill",             setup.min_via_drill_mm);
    add_num ("min_hole_clearance",        setup.min_hole_clearance_mm);
    add_num ("min_hole_to_hole",          setup.min_hole_to_hole_mm);
    add_num ("min_copper_edge_clearance", setup.min_copper_edge_clearance_mm);
    add_bool("allow_blind_buried_vias",   setup.allow_blind_buried_vias);
    add_bool("allow_microvias",           setup.allow_microvias);
    board.raw_setup_sexpr = sexpr::to_kicad_string(*root);
}

std::string to_json(const Setup & s) {
    json j = json::object();
    json classes = json::array();
    for (const auto & c : s.classes) {
        json cj;
        cj["name"] = c.name;
        cj["clearance_mm"]        = c.clearance_mm;
        cj["track_width_mm"]      = c.track_width_mm;
        cj["via_diameter_mm"]     = c.via_diameter_mm;
        cj["via_drill_mm"]        = c.via_drill_mm;
        cj["uvia_diameter_mm"]    = c.uvia_diameter_mm;
        cj["uvia_drill_mm"]       = c.uvia_drill_mm;
        cj["diff_pair_width_mm"]  = c.diff_pair_width_mm;
        cj["diff_pair_gap_mm"]    = c.diff_pair_gap_mm;
        cj["patterns"] = c.patterns;
        classes.push_back(cj);
    }
    j["classes"] = classes;
    json layers = json::array();
    for (const auto & L : s.stackup.layers) {
        layers.push_back({{"id",L.id},{"canonical",L.canonical_name},
                          {"user",L.user_name},{"type",L.type}});
    }
    j["layers"]              = layers;
    j["board_thickness_mm"]  = s.stackup.board_thickness_mm;
    j["min_clearance_mm"]    = s.min_clearance_mm;
    j["min_track_width_mm"]  = s.min_track_width_mm;
    j["min_via_diameter_mm"] = s.min_via_diameter_mm;
    j["min_via_drill_mm"]    = s.min_via_drill_mm;
    j["min_hole_clearance_mm"]        = s.min_hole_clearance_mm;
    j["min_hole_to_hole_mm"]          = s.min_hole_to_hole_mm;
    j["min_copper_edge_clearance_mm"] = s.min_copper_edge_clearance_mm;
    j["allow_blind_buried_vias"]      = s.allow_blind_buried_vias;
    j["allow_microvias"]              = s.allow_microvias;
    return j.dump(2);
}

Setup from_json(std::string_view text) {
    Setup s;
    auto j = json::parse(text, nullptr, false);
    if (j.is_discarded() || !j.is_object()) return s;
    if (j.contains("classes") && j["classes"].is_array()) {
        s.classes.clear();
        for (const auto & cj : j["classes"]) {
            NetClass c;
            c.name              = cj.value("name",              std::string("Default"));
            c.clearance_mm      = cj.value("clearance_mm",       0.15);
            c.track_width_mm    = cj.value("track_width_mm",     0.2);
            c.via_diameter_mm   = cj.value("via_diameter_mm",    0.6);
            c.via_drill_mm      = cj.value("via_drill_mm",       0.3);
            c.uvia_diameter_mm  = cj.value("uvia_diameter_mm",   0.3);
            c.uvia_drill_mm     = cj.value("uvia_drill_mm",      0.1);
            c.diff_pair_width_mm= cj.value("diff_pair_width_mm", 0.2);
            c.diff_pair_gap_mm  = cj.value("diff_pair_gap_mm",   0.25);
            if (cj.contains("patterns") && cj["patterns"].is_array())
                for (const auto & p : cj["patterns"]) c.patterns.push_back(p.get<std::string>());
            s.classes.push_back(c);
        }
    }
    if (j.contains("layers") && j["layers"].is_array()) {
        s.stackup.layers.clear();
        for (const auto & lj : j["layers"]) {
            kicad_model::LayerInfo L;
            L.id             = lj.value("id", 0);
            L.canonical_name = lj.value("canonical", std::string("F.Cu"));
            L.user_name      = lj.value("user", L.canonical_name);
            L.type           = lj.value("type", std::string("signal"));
            s.stackup.layers.push_back(L);
        }
    }
    s.stackup.board_thickness_mm   = j.value("board_thickness_mm",   1.6);
    s.min_clearance_mm             = j.value("min_clearance_mm",     0.15);
    s.min_track_width_mm           = j.value("min_track_width_mm",   0.15);
    s.min_via_diameter_mm          = j.value("min_via_diameter_mm",  0.4);
    s.min_via_drill_mm             = j.value("min_via_drill_mm",     0.2);
    s.min_hole_clearance_mm        = j.value("min_hole_clearance_mm",       0.25);
    s.min_hole_to_hole_mm          = j.value("min_hole_to_hole_mm",         0.25);
    s.min_copper_edge_clearance_mm = j.value("min_copper_edge_clearance_mm",0.3);
    s.allow_blind_buried_vias      = j.value("allow_blind_buried_vias",     false);
    s.allow_microvias              = j.value("allow_microvias",             false);
    return s;
}

} // namespace board_setup
