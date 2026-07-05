// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// IBIS model parser (IEC 62014-1 / IBIS 5.x / 7.x).
// Text-based ASCII format used to describe I/O buffer behavior of
// digital IC pins for signal-integrity simulation.
//
// MVP scope: parse the header block and any [Model] sections up to
// their voltage/current tables. Sufficient to drive a KiCad SIM_MODEL
// picker and to feed a follow-up simulator. Waveform tables are stored
// as raw (time, voltage) pairs.
namespace ibis {

struct HeaderInfo {
    std::string ibis_ver;         // e.g. "5.1"
    std::string file_name;
    std::string file_rev;
    std::string date;
    std::string source;
    std::string notes;
    std::string disclaimer;
    std::string copyright;
    std::string component;        // first [Component] name we see
    std::string manufacturer;
    std::string package_name;
};

struct Pin {
    std::string name;             // e.g. "1", "A5"
    std::string signal_name;      // e.g. "VDD", "GPIO0"
    std::string model_name;       // matches a [Model] block
    double      r_pin_ohm = 0.0;
    double      l_pin_nH  = 0.0;
    double      c_pin_pF  = 0.0;
};

struct IVTable {
    std::string kind;             // "Pulldown","Pullup","POWER Clamp","GND Clamp"
    std::vector<std::pair<double,double>> points; // (V, I) typ column
};

struct VTTable {
    std::string kind;             // "Rising Waveform", "Falling Waveform"
    std::vector<std::pair<double,double>> points; // (t, V)
};

struct Model {
    std::string name;
    std::string model_type;       // e.g. "I/O","Input","Output"
    double c_comp_pF = 0.0;
    double vinh = 0.0, vinl = 0.0;
    std::vector<IVTable> iv;
    std::vector<VTTable> vt;
};

struct File {
    HeaderInfo header;
    std::vector<Pin> pins;
    std::vector<Model> models;
    std::vector<std::string> warnings;
};

std::optional<File> parse(std::string_view text);
std::optional<File> parse_file(std::string_view path);

}
