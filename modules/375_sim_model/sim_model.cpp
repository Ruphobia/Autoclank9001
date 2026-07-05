// SPDX-License-Identifier: GPL-3.0-or-later
#include "sim_model.hpp"

#include <nlohmann/json.hpp>

#include <cctype>
#include <sstream>

namespace sim_model {

using json = nlohmann::json;

namespace {

const char * kind_name(Kind k) {
    switch (k) {
        case Kind::Resistor:      return "R";
        case Kind::Capacitor:     return "C";
        case Kind::Inductor:      return "L";
        case Kind::VoltageSource: return "V";
        case Kind::CurrentSource: return "I";
        case Kind::Diode:         return "D";
        case Kind::Bjt:           return "Q";
        case Kind::Mosfet:        return "M";
        case Kind::Subckt:        return "X";
        case Kind::RawSpice:      return "*";
    }
    return "*";
}

Kind kind_from_letter(char c) {
    switch (std::toupper(c)) {
        case 'R': return Kind::Resistor;
        case 'C': return Kind::Capacitor;
        case 'L': return Kind::Inductor;
        case 'V': return Kind::VoltageSource;
        case 'I': return Kind::CurrentSource;
        case 'D': return Kind::Diode;
        case 'Q': return Kind::Bjt;
        case 'M': return Kind::Mosfet;
        case 'X': return Kind::Subckt;
        default:  return Kind::RawSpice;
    }
}

} // namespace

std::string to_spice(const Model & m) {
    std::ostringstream os;
    if (m.kind == Kind::RawSpice) {
        os << m.raw_card;
        if (!m.raw_card.empty() && m.raw_card.back() != '\n') os << "\n";
        return os.str();
    }
    // Element card. Behavioral suffix appended if present.
    os << m.ref << " " << m.value;
    if (!m.behavioral_expr.empty()) os << " " << m.behavioral_expr;
    os << "\n";
    // Optional .MODEL card for semiconductors.
    if (!m.model_name.empty()) {
        os << ".MODEL " << m.model_name << " ";
        switch (m.kind) {
            case Kind::Diode:   os << "D";     break;
            case Kind::Bjt:     os << "NPN";   break; // default; user can override via params
            case Kind::Mosfet:  os << "NMOS";  break;
            default:            os << "SUB";   break;
        }
        os << "(";
        bool first = true;
        for (const auto & p : m.params) {
            if (!first) os << " ";
            os << p.first << "=" << p.second;
            first = false;
        }
        os << ")\n";
    }
    return os.str();
}

std::string to_json(const Model & m) {
    json j;
    j["kind"] = static_cast<int>(m.kind);
    j["ref"] = m.ref;
    j["value"] = m.value;
    j["model_name"] = m.model_name;
    j["raw_card"] = m.raw_card;
    j["behavioral_expr"] = m.behavioral_expr;
    json p = json::array();
    for (const auto & kv : m.params) p.push_back({{"key",kv.first},{"value",kv.second}});
    j["params"] = p;
    return j.dump();
}

Model from_json(std::string_view text) {
    Model m;
    auto j = json::parse(text, nullptr, false);
    if (!j.is_object()) return m;
    m.kind = static_cast<Kind>(j.value("kind", 0));
    m.ref  = j.value("ref", "");
    m.value = j.value("value", "");
    m.model_name = j.value("model_name", "");
    m.raw_card   = j.value("raw_card", "");
    m.behavioral_expr = j.value("behavioral_expr", "");
    if (j.contains("params") && j["params"].is_array()) {
        for (const auto & p : j["params"])
            m.params.emplace_back(p.value("key",""), p.value("value",""));
    }
    return m;
}

std::vector<Model> catalog() {
    std::vector<Model> out;
    out.push_back({ Kind::Resistor,       "R?", "10k",  "", {}, "", "" });
    out.push_back({ Kind::Capacitor,      "C?", "10uF", "", {}, "", "" });
    out.push_back({ Kind::Inductor,       "L?", "10uH", "", {}, "", "" });
    out.push_back({ Kind::VoltageSource,  "V?", "PULSE(0 5 0 1n 1n 500u 1m)", "", {}, "", "" });
    out.push_back({ Kind::CurrentSource,  "I?", "10mA", "", {}, "", "" });
    Model diode;   diode.kind = Kind::Diode;
    diode.ref = "D?"; diode.value = "DMOD";
    diode.model_name = "DMOD";
    diode.params = { {"IS","1e-14"}, {"N","1.0"}, {"BV","50"} };
    out.push_back(diode);
    Model bjt;     bjt.kind = Kind::Bjt;
    bjt.ref = "Q?"; bjt.value = "QMOD";
    bjt.model_name = "QMOD";
    bjt.params = { {"BF","150"}, {"VAF","100"} };
    out.push_back(bjt);
    Model mos;     mos.kind = Kind::Mosfet;
    mos.ref = "M?"; mos.value = "MMOD";
    mos.model_name = "MMOD";
    mos.params = { {"VTO","1.0"}, {"KP","20u"} };
    out.push_back(mos);
    return out;
}

std::optional<Model> parse_spice_line(std::string_view line) {
    while (!line.empty() && std::isspace(static_cast<unsigned char>(line.front()))) line.remove_prefix(1);
    if (line.empty() || line[0] == '*' || line[0] == '.') return std::nullopt;

    Model m;
    m.kind = kind_from_letter(line[0]);
    // Extract tokens.
    std::vector<std::string> tk;
    std::size_t i = 0;
    while (i < line.size()) {
        while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        std::size_t s = i;
        while (i < line.size() && !std::isspace(static_cast<unsigned char>(line[i]))) ++i;
        if (i > s) tk.emplace_back(line.substr(s, i - s));
    }
    if (tk.empty()) return std::nullopt;
    m.ref = tk[0];
    if (tk.size() >= 4) m.value = tk[3];   // Nodes are 1..2, value is 3rd index for R/L/C
    else if (tk.size() >= 2) m.value = tk.back();
    return m;
}

} // namespace sim_model
