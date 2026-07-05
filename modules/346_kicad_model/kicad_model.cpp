// SPDX-License-Identifier: GPL-3.0-or-later
#include "kicad_model.hpp"

#include <cstdio>
#include <cstdint>
#include <random>

namespace kicad_model {

UUID make_uuid() {
    static thread_local std::mt19937_64 rng{std::random_device{}()};
    std::uniform_int_distribution<std::uint64_t> d;
    std::uint64_t a = d(rng), b = d(rng);
    a = (a & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    b = (b & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    char buf[37];
    std::snprintf(buf, sizeof(buf),
                  "%08lx-%04lx-%04lx-%04lx-%012lx",
                  static_cast<unsigned long>((a >> 32) & 0xFFFFFFFFULL),
                  static_cast<unsigned long>((a >> 16) & 0xFFFFULL),
                  static_cast<unsigned long>(a & 0xFFFFULL),
                  static_cast<unsigned long>((b >> 48) & 0xFFFFULL),
                  static_cast<unsigned long>(b & 0xFFFFFFFFFFFFULL));
    return UUID(buf, 36);
}

std::string SchSymbol::field_value(std::string_view name) const {
    for (const auto & f : fields) if (f.name == name) return f.value;
    return {};
}

int intern_net(Board & b, std::string_view name) {
    for (const auto & n : b.nets) if (n.name == name) return n.id;
    NetInfo n; n.id = static_cast<int>(b.nets.size()); n.name = std::string(name);
    b.nets.push_back(n);
    return n.id;
}

std::vector<LayerInfo> default_2layer_stackup() {
    return {
        { 0, "F.Cu",     "F.Cu",           "signal"},
        { 2, "B.Cu",     "B.Cu",           "signal"},
        { 9, "F.Adhes",  "F.Adhesive",     "user"},
        {11, "B.Adhes",  "B.Adhesive",     "user"},
        {13, "F.Paste",  "F.Paste",        "user"},
        {15, "B.Paste",  "B.Paste",        "user"},
        { 5, "F.SilkS",  "F.Silkscreen",   "user"},
        { 7, "B.SilkS",  "B.Silkscreen",   "user"},
        { 1, "F.Mask",   "F.Mask",         "user"},
        { 3, "B.Mask",   "B.Mask",         "user"},
        {17, "Dwgs.User","User.Drawings",  "user"},
        {19, "Cmts.User","User.Comments",  "user"},
        {21, "Eco1.User","User.Eco1",      "user"},
        {23, "Eco2.User","User.Eco2",      "user"},
        {25, "Edge.Cuts","Edge.Cuts",      "user"},
        {27, "Margin",   "Margin",         "user"},
        {31, "F.CrtYd",  "F.Courtyard",    "user"},
        {29, "B.CrtYd",  "B.Courtyard",    "user"},
        {35, "F.Fab",    "F.Fab",          "user"},
        {33, "B.Fab",    "B.Fab",          "user"}
    };
}

} // namespace kicad_model
