// SPDX-License-Identifier: GPL-3.0-or-later
#include "kicad_project.hpp"

#include <nlohmann/json.hpp>

#include <cmath>
#include <string>

namespace kicad_project {
namespace {

using json = nlohmann::json;

constexpr double MIL_TO_MM = 0.0254;

json empty_dict()  { return json::object(); }
json empty_array() { return json::array(); }

// KiCad's DRC severity defaults distilled from the microwave demo.
// Anything unspecified is left out (KiCad substitutes its own default).
json default_drc_severities() {
    return {
        {"annular_width",                    "error"},
        {"clearance",                        "error"},
        {"connection_width",                 "warning"},
        {"copper_edge_clearance",            "error"},
        {"copper_sliver",                    "warning"},
        {"courtyards_overlap",               "error"},
        {"creepage",                         "error"},
        {"diff_pair_gap_out_of_range",       "error"},
        {"diff_pair_uncoupled_length_too_long", "error"},
        {"drill_out_of_range",               "error"},
        {"duplicate_footprints",             "warning"},
        {"extra_footprint",                  "warning"},
        {"footprint",                        "error"},
        {"footprint_filters_mismatch",       "ignore"},
        {"footprint_symbol_mismatch",        "warning"},
        {"footprint_type_mismatch",          "ignore"},
        {"hole_clearance",                   "error"},
        {"hole_near_hole",                   "error"},
        {"hole_to_hole",                     "error"},
        {"holes_co_located",                 "warning"},
        {"invalid_outline",                  "error"},
        {"isolated_copper",                  "warning"},
        {"item_on_disabled_layer",           "error"},
        {"items_not_allowed",                "error"},
        {"length_out_of_range",              "error"},
        {"lib_footprint_issues",             "warning"},
        {"lib_footprint_mismatch",           "warning"},
        {"malformed_courtyard",              "error"},
        {"missing_courtyard",                "ignore"},
        {"missing_footprint",                "warning"},
        {"npth_inside_courtyard",            "ignore"},
        {"padstack",                         "warning"},
        {"pth_inside_courtyard",             "ignore"},
        {"shorting_items",                   "error"},
        {"silk_edge_clearance",              "warning"},
        {"silk_over_copper",                 "warning"},
        {"silk_overlap",                     "warning"},
        {"skew_out_of_range",                "error"},
        {"solder_mask_bridge",               "error"},
        {"starved_thermal",                  "error"},
        {"text_height",                      "warning"},
        {"text_thickness",                   "warning"},
        {"through_hole_pad_without_hole",    "error"},
        {"too_many_vias",                    "error"},
        {"track_angle",                      "ignore"},
        {"track_dangling",                   "warning"},
        {"track_segment_length",             "ignore"},
        {"track_width",                      "error"},
        {"tracks_crossing",                  "error"},
        {"unconnected_items",                "error"},
        {"unresolved_variable",              "error"},
        {"via_dangling",                     "warning"},
        {"zone_has_empty_net",               "error"},
        {"zones_intersect",                  "error"}
    };
}

// KiCad's ERC severity defaults, similarly.
json default_erc_severities() {
    return {
        {"bus_definition_conflict",       "error"},
        {"bus_entry_needed",              "error"},
        {"bus_to_bus_conflict",           "error"},
        {"bus_to_net_conflict",           "error"},
        {"conflicting_netclasses",        "error"},
        {"different_unit_footprint",      "error"},
        {"different_unit_net",            "error"},
        {"duplicate_reference",           "error"},
        {"duplicate_sheet_names",         "error"},
        {"endpoint_off_grid",             "warning"},
        {"extra_units",                   "error"},
        {"footprint_link_issues",         "warning"},
        {"global_label_dangling",         "warning"},
        {"hier_label_mismatch",           "error"},
        {"label_dangling",                "error"},
        {"lib_symbol_issues",             "warning"},
        {"lib_symbol_mismatch",           "warning"},
        {"missing_bidi_pin",              "warning"},
        {"missing_input_pin",             "warning"},
        {"missing_power_pin",             "error"},
        {"missing_unit",                  "warning"},
        {"multiple_net_names",            "warning"},
        {"net_not_bus_member",            "warning"},
        {"no_connect_connected",          "warning"},
        {"no_connect_dangling",           "warning"},
        {"pin_not_connected",             "error"},
        {"pin_not_driven",                "error"},
        {"pin_to_pin",                    "warning"},
        {"power_pin_not_driven",          "error"},
        {"similar_labels",                "warning"},
        {"simulation_model_issue",        "ignore"},
        {"single_global_label",           "info"},
        {"unannotated",                   "error"},
        {"unresolved_variable",           "error"},
        {"wire_dangling",                 "error"}
    };
}

// KiCad's 12x12 pin-connection matrix. Values: 0=OK, 1=warning, 2=error.
// Rows/cols: input, output, bidirectional, tri-state, passive, unspecified,
// power-in, power-out, open-collector, open-emitter, no-connect, no-connection.
// Copied from KiCad's built-in default.
json default_erc_pin_map() {
    // Order rebuilt against KiCad's ERC defaults so the emitted project
    // matches what eeschema fills in.
    return json::array({
        json::array({0,0,0,0,0,0,0,2,0,0,0,0}),  // input
        json::array({0,2,0,1,0,0,0,2,2,2,2,0}),  // output
        json::array({0,0,0,0,0,0,0,2,1,1,2,0}),  // bidi
        json::array({0,1,0,0,0,0,0,2,1,1,2,0}),  // tri-state
        json::array({0,0,0,0,0,0,0,2,0,0,2,0}),  // passive
        json::array({0,0,0,0,0,0,0,0,0,0,2,0}),  // unspecified
        json::array({0,0,0,0,0,0,0,2,0,0,2,0}),  // power-in
        json::array({2,2,2,2,2,0,2,2,2,2,2,0}),  // power-out
        json::array({0,2,1,1,0,0,0,2,1,2,2,0}),  // open-collector
        json::array({0,2,1,1,0,0,0,2,2,1,2,0}),  // open-emitter
        json::array({0,2,2,2,2,2,2,2,2,2,2,0}),  // no-connect
        json::array({0,0,0,0,0,0,0,0,0,0,0,0})   // no-connection
    });
}

json netclass_to_json(const NetClass & c) {
    json j = json::object();
    j["name"]                    = c.name;
    j["clearance"]               = c.clearance_mm;
    j["track_width"]             = c.track_width_mm;
    j["via_diameter"]            = c.via_diameter_mm;
    j["via_drill"]               = c.via_drill_mm;
    j["microvia_diameter"]       = c.uvia_diameter_mm;
    j["microvia_drill"]          = c.uvia_drill_mm;
    j["diff_pair_width"]         = c.diff_pair_width_mm;
    j["diff_pair_gap"]           = c.diff_pair_gap_mm;
    j["diff_pair_via_gap"]       = c.diff_pair_via_gap_mm;
    j["pcb_color"]               = "rgba(0,0,0,0.000)";
    j["schematic_color"]         = "rgba(0,0,0,0.000)";
    j["line_style"]              = 0;
    j["wire_width"]              = 6.0;
    j["bus_width"]               = 12.0;
    j["priority"]                = 0;
    return j;
}

} // namespace

FabProfile builtin_profile(std::string_view name) {
    FabProfile p;
    p.name = "jlcpcb_default";
    p.trace_width_mil = 6;
    p.clearance_mil   = 6;
    p.via_drill_mm    = 0.3;
    p.via_diameter_mm = 0.6;
    p.board_edge_clearance_mm = 0.3;

    if (name == "jlcpcb_4layer") {
        p.name = "jlcpcb_4layer";
        p.trace_width_mil = 4;
        p.clearance_mil   = 4;
    } else if (name == "oshpark_default") {
        p.name = "oshpark_default";
        p.trace_width_mil = 5;
        p.clearance_mil   = 5;
    } else if (name == "pcbway_default") {
        p.name = "pcbway_default";
        p.trace_width_mil = 5;
        p.clearance_mil   = 5;
    }
    return p;
}

Project default_project(std::string_view title) {
    Project pj;
    pj.title = std::string(title);
    pj.copper_layers = 2;
    pj.fab_profile   = builtin_profile("jlcpcb_default");

    NetClass def;
    def.name = "Default";
    def.clearance_mm     = pj.fab_profile.clearance_mil   * MIL_TO_MM;
    def.track_width_mm   = pj.fab_profile.trace_width_mil * MIL_TO_MM;
    def.via_diameter_mm  = pj.fab_profile.via_diameter_mm;
    def.via_drill_mm     = pj.fab_profile.via_drill_mm;
    pj.netclasses.push_back(def);

    NetClass power = def;
    power.name             = "Power";
    power.track_width_mm   = 0.5;   // 20mil
    power.clearance_mm     = 0.25;
    power.via_diameter_mm  = 0.8;
    power.via_drill_mm     = 0.4;
    power.patterns         = {"^\\+.*", "^-.*", "GND", "VBAT", "VCC", "VDD", "VSS"};
    pj.netclasses.push_back(power);

    return pj;
}

std::string to_json(const Project & in, int indent) {
    json root = json::object();

    // meta
    root["meta"] = { {"filename", "kicad.kicad_pro"}, {"version", in.schema_version} };
    root["sheets"] = empty_array();
    root["text_variables"] = empty_dict();
    root["boards"] = empty_array();

    // board section
    json board = json::object();
    board["3dviewports"]    = empty_array();
    board["layer_pairs"]    = empty_array();
    board["layer_presets"]  = empty_array();
    board["viewports"]      = empty_array();

    json ds = json::object();
    ds["meta"] = { {"filename", "board_design_settings.json"}, {"version", 2} };
    ds["rules"] = {
        {"allow_blind_buried_vias", false},
        {"allow_microvias",         false},
        {"min_clearance",           in.netclasses.empty() ? 0.15 : in.netclasses[0].clearance_mm},
        {"min_copper_edge_clearance", in.fab_profile.board_edge_clearance_mm},
        {"min_hole_clearance",      0.25},
        {"min_hole_to_hole",        0.25},
        {"min_microvia_diameter",   in.netclasses.empty() ? 0.2  : in.netclasses[0].uvia_diameter_mm},
        {"min_microvia_drill",      in.netclasses.empty() ? 0.1  : in.netclasses[0].uvia_drill_mm},
        {"min_silk_clearance",      0.15},
        {"min_text_height",         0.8},
        {"min_text_thickness",      0.08},
        {"min_through_hole_diameter", 0.3},
        {"min_track_width",         in.netclasses.empty() ? 0.15 : in.netclasses[0].track_width_mm},
        {"min_via_annular_width",   0.1},
        {"min_via_diameter",        in.netclasses.empty() ? 0.4  : in.netclasses[0].via_diameter_mm},
        {"solder_mask_to_copper_clearance", 0.0},
        {"use_height_for_length_calcs", true}
    };
    ds["defaults"] = {
        {"apply_defaults_to_fp_fields", false},
        {"apply_defaults_to_fp_shapes", false},
        {"apply_defaults_to_fp_text",   false},
        {"board_outline_line_width",    0.1},
        {"copper_line_width",           0.2},
        {"copper_text_italic",          false},
        {"copper_text_size_h",          1.5},
        {"copper_text_size_v",          1.5},
        {"copper_text_thickness",       0.3},
        {"copper_text_upright",         false},
        {"courtyard_line_width",        0.05},
        {"dimension_precision",         4},
        {"dimension_units",             3},
        {"dimensions", {
            {"arrow_length", 1270000},
            {"extension_offset", 500000},
            {"keep_text_aligned", true},
            {"suppress_zeroes", false},
            {"text_position", 0},
            {"units_format", 1}
        }},
        {"fab_line_width",    0.1},
        {"fab_text_italic",   false},
        {"fab_text_size_h",   1.0},
        {"fab_text_size_v",   1.0},
        {"fab_text_thickness",0.15},
        {"fab_text_upright",  false},
        {"other_line_width",  0.1},
        {"other_text_italic", false},
        {"other_text_size_h", 1.0},
        {"other_text_size_v", 1.0},
        {"other_text_thickness", 0.15},
        {"other_text_upright", false},
        {"pads", {{"drill", 0.762}, {"height", 1.524}, {"width", 1.524}}},
        {"silk_line_width",     0.12},
        {"silk_text_italic",    false},
        {"silk_text_size_h",    1.0},
        {"silk_text_size_v",    1.0},
        {"silk_text_thickness", 0.15},
        {"silk_text_upright",   false},
        {"zones", {{"min_clearance", 0.25}}}
    };
    ds["diff_pair_dimensions"] = empty_array();
    ds["drc_exclusions"]       = empty_array();
    ds["rule_severities"]      = default_drc_severities();
    ds["rules_report_directives"] = empty_array();
    ds["track_widths"]         = empty_array();
    ds["tuning_pattern_settings"] = json::object();
    ds["via_dimensions"]       = empty_array();
    ds["zones_allow_external_fillets"] = false;
    board["design_settings"]   = ds;
    board["ipc2581"] = {
        {"dist", ""}, {"distpn", ""}, {"internal_id", ""},
        {"mfg", ""},  {"mpn", ""}
    };
    root["board"] = std::move(board);

    // cvpcb
    root["cvpcb"] = { {"equivalence_files", empty_array()} };

    // erc
    root["erc"] = {
        {"erc_exclusions",   empty_array()},
        {"meta",             {{"version", 0}}},
        {"pin_map",          default_erc_pin_map()},
        {"rule_severities",  default_erc_severities()}
    };

    // libraries
    root["libraries"] = {
        {"pinned_footprint_libs", empty_array()},
        {"pinned_symbol_libs",    empty_array()}
    };

    // net_settings
    json classes = json::array();
    for (const auto & c : in.netclasses) classes.push_back(netclass_to_json(c));
    json patterns = json::array();
    for (const auto & c : in.netclasses) {
        for (const auto & p : c.patterns) {
            patterns.push_back({ {"netclass", c.name}, {"pattern", p} });
        }
    }
    root["net_settings"] = {
        {"classes",              std::move(classes)},
        {"meta",                 {{"version", 4}}},
        {"net_colors",           nullptr},
        {"netclass_assignments", nullptr},
        {"netclass_patterns",    std::move(patterns)}
    };

    // pcbnew
    root["pcbnew"] = {
        {"last_paths", {
            {"gencad", ""}, {"idf", ""}, {"netlist", ""},
            {"plot",   ""}, {"pos_files", ""}, {"specctra_dsn", ""},
            {"step",   ""}, {"svg",  ""}, {"vrml", ""}
        }},
        {"page_layout_descr_file", ""}
    };

    // schematic
    root["schematic"] = {
        {"drawing", {
            {"dashed_lines_dash_length_ratio", 12.0},
            {"dashed_lines_gap_length_ratio",  3.0},
            {"default_bus_thickness",          12.0},
            {"default_junction_size",          40.0},
            {"default_line_thickness",         6.0},
            {"default_text_size",              50.0},
            {"default_wire_thickness",         6.0},
            {"field_names",                    empty_array()},
            {"intersheets_ref_own_page",       false},
            {"intersheets_ref_prefix",         ""},
            {"intersheets_ref_short",          false},
            {"intersheets_ref_show",           false},
            {"intersheets_ref_suffix",         ""},
            {"junction_size_choice",           3},
            {"label_size_ratio",               0.375},
            {"operating_point_overlay_i_precision", 3},
            {"operating_point_overlay_i_range", "~A"},
            {"operating_point_overlay_v_precision", 3},
            {"operating_point_overlay_v_range", "~V"},
            {"overbar_offset_ratio",           1.23},
            {"pin_symbol_size",                25.0},
            {"text_offset_ratio",              0.15}
        }},
        {"legacy_lib_dir",           ""},
        {"legacy_lib_list",          empty_array()},
        {"meta",                     {{"version", 1}}},
        {"net_format_name",          ""},
        {"page_layout_descr_file",   ""},
        {"plot_directory",           ""},
        {"spice_adjust_passive_values", false},
        {"spice_external_command",   "spice \"%I\""},
        {"subpart_first_id",         65},
        {"subpart_id_separator",     0}
    };

    return root.dump(indent);
}

std::string to_prl_json(const Project & in) {
    (void) in;
    json j = {
        {"board", {
            {"active_layer", 0},
            {"active_layer_preset", ""},
            {"auto_track_width", true},
            {"hidden_netclasses", empty_array()},
            {"hidden_nets",       empty_array()},
            {"high_contrast_mode", 0},
            {"net_color_mode",     1},
            {"opacity", {
                {"images", 0.6},{"pads", 1.0},{"shapes", 1.0},
                {"tracks", 1.0},{"vias",  1.0},{"zones",  0.6}
            }},
            {"prototype_zone_fills", false},
            {"selection_filter", {
                {"dimensions", true},{"footprints", true},{"graphics", true},
                {"keepouts",  true},{"lockedItems", false},{"otherItems", true},
                {"pads",       true},{"text",       true},{"tracks",     true},
                {"vias",       true},{"zones",      true}
            }}
        }},
        {"meta", {{"filename", "kicad.kicad_prl"}, {"version", 3}}},
        {"project", {{"files", empty_array()}}}
    };
    return j.dump(2);
}

} // namespace kicad_project
