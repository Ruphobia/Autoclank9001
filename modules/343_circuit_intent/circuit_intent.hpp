// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Circuit intent is the stable intermediate the AI produces and every
// downstream stage (schematic emitter, PCB emitter, SPICE netlister)
// consumes. Small enough for a small model to synthesize cleanly; rich
// enough that KiCad-compatible files can be generated from it.
//
// Schema shape mirrors scratchpad/kicad_integration.md §6.
namespace circuit_intent {

// A single power/reference net. Constrained so the design has a spine
// the AI can wire everything else against.
struct PowerNet {
    std::string name;       // "+5V", "+9V", "GND", "VBAT", ...
    double      voltage = 0.0; // ground plane == 0.0
};

// One part in the design. `lib_hint` and `footprint_hint` are AI
// suggestions; 341_kicad_libs may substitute better matches. When the
// AI is confident (`lib_hint_locked = true`) the emitter fails rather
// than silently substituting.
struct Part {
    std::string ref;             // "U1", "R3", "C2"
    std::string value;           // "NE555", "33k", "10nF"
    std::string lib_hint;        // "Timer:NE555" (lib:name)
    std::string footprint_hint;  // "Package_DIP:DIP-8_W7.62mm" (lib:name)
    std::string mpn;             // manufacturer part number (from Mouser)
    std::string manufacturer;
    std::string datasheet_url;
    bool        dnp = false;     // "do not populate"
    bool        lib_hint_locked = false;
};

// A single node in the connection graph. Refers to either a part pin
// ("U1.VCC", "R1.2") or a named net ("+9V", "GND").
struct Node {
    std::string endpoint;
};

// One net: a set of endpoints that must be electrically connected.
// When the connection list is dense we flatten to a set of two-endpoint
// edges via `pair_edges()`; both forms round-trip.
struct Net {
    std::string       name;      // may be empty (unnamed local net)
    std::vector<Node> endpoints;
    std::string       netclass;  // "Default", "Power", ... (see kicad_project)
};

// Board-level hints.
struct BoardHints {
    double  width_mm         = 50.0;
    double  height_mm        = 30.0;
    int     copper_layers    = 2;
    std::string fab_profile  = "jlcpcb_default"; // netclass preset name
};

struct PlacementGroup {
    std::vector<std::string> refs;  // ["R1","R2","C1"]
    std::string              near;  // "U1"  (place group adjacent to this ref)
};

struct Intent {
    // Metadata
    std::string title;
    std::string notes;

    // Design
    std::vector<PowerNet>       power_nets;
    std::vector<Part>           parts;
    std::vector<Net>            nets;
    BoardHints                  board;
    std::vector<PlacementGroup> placement_hints;
};

// --- serialization ---

// Parse a JSON intent (as produced by the AI or hand-authored fixtures).
// On failure, `error_out` receives the offending pointer and reason.
bool parse_json(std::string_view json, Intent & out, std::string & error_out);

// Emit intent as JSON (round-trippable with parse_json).
std::string to_json(const Intent & in, int indent = 2);

// --- validation ---

struct Diagnostic {
    enum class Severity { Error, Warning, Info };
    Severity    severity;
    std::string field;     // "parts[3].ref", "nets[7].endpoints[1]"
    std::string message;
};

// Structural checks that don't require an external library index:
//   * every ref-designator is unique
//   * every endpoint refers to either a known ref or a named net
//   * every named net referenced has a defining PowerNet or Net entry
//   * ref-designators match [A-Z]+[0-9]+
//   * board dimensions positive and finite
std::vector<Diagnostic> validate(const Intent & in);

// --- convenience ---

// Flatten Net::endpoints into deduped edges (u,v) for consumers that want
// pair form (netlister, ratsnest, router).
struct Edge { std::string a; std::string b; std::string net; };
std::vector<Edge> pair_edges(const Intent & in);

// Split "U1.VCC" -> ("U1", "VCC"). Returns false when there is no dot.
bool split_endpoint(std::string_view ep, std::string & ref_out, std::string & pin_out);

}
