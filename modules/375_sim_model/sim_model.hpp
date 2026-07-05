// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// SPICE SIM_MODEL editor: build up simulation model cards for
// resistors, capacitors, inductors, sources, and semiconductor
// devices, then emit the matching SPICE .MODEL / behavioral lines.
//
// Not a full BSIM/HICUM/VBIC/PSP frontend; supports the common cases
// (R/L/C/V/I values, diode/BJT/MOSFET model parameters as key=value
// pairs, subcircuit include). Complex process-model editing is a
// follow-up.
namespace sim_model {

enum class Kind {
    Resistor,
    Capacitor,
    Inductor,
    VoltageSource,
    CurrentSource,
    Diode,
    Bjt,
    Mosfet,
    Subckt,
    RawSpice        // free-form user-supplied model card
};

struct Model {
    Kind kind = Kind::Resistor;
    std::string ref;        // "R1", "Q3"
    std::string value;      // "10k" / "10uF" / model name for semiconductors
    std::string model_name; // ".MODEL <name> <type>(...)"
    std::vector<std::pair<std::string, std::string>> params; // model params (key=value)
    std::string raw_card;   // used when kind == RawSpice
    // Behavioral source expression, e.g. "V=V(a)*V(b)" for BSOURCE.
    std::string behavioral_expr;
};

// Emit one or more SPICE lines that represent this Model.
std::string to_spice(const Model & m);

// JSON round-trip.
std::string to_json  (const Model & m);
Model       from_json(std::string_view text);

// A catalog of predefined defaults for common devices.
std::vector<Model> catalog();

// Parse a bare SPICE line ("R1 a b 10k") into a Model. Best-effort;
// returns nullopt for unrecognized syntax.
std::optional<Model> parse_spice_line(std::string_view line);

}
