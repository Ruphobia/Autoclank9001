// SPDX-License-Identifier: GPL-3.0-or-later
#include "pos_overrides.hpp"

#include <nlohmann/json.hpp>

#include <sstream>

namespace pos_overrides {

using json = nlohmann::json;
using kicad_model::Board;
using kicad_model::Footprint;
using kicad_model::ItemType;

std::string write_pos_csv(const Board & board, const Table & overrides) {
    std::ostringstream os;
    os << "Ref,Value,Package,PosX,PosY,Rot,Side\n";
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint*>(it.get());
        std::string ref, val;
        for (const auto & f : fp->fields) {
            if (f.name == "Reference") ref = f.value;
            if (f.name == "Value")     val = f.value;
        }
        double posx = geom::nm_to_mm(fp->at.x);
        double posy = geom::nm_to_mm(fp->at.y);
        double rot  = fp->angle.deg();
        bool   side_bottom = fp->placement_layer == "B.Cu";
        auto it_ov = overrides.find(ref);
        if (it_ov != overrides.end()) {
            posx += it_ov->second.dx_mm;
            posy += it_ov->second.dy_mm;
            rot  += it_ov->second.rotation_delta_deg;
            if (it_ov->second.flip_side) side_bottom = !side_bottom;
        }
        os << ref << "," << val << "," << fp->lib_id << ","
           << posx << "," << posy << ","
           << rot << ","
           << (side_bottom ? "bottom" : "top") << "\n";
    }
    return os.str();
}

std::string to_json(const Table & t) {
    json j = json::object();
    for (const auto & kv : t) {
        j[kv.first] = {
            {"rot_delta", kv.second.rotation_delta_deg},
            {"flip",      kv.second.flip_side},
            {"dx",        kv.second.dx_mm},
            {"dy",        kv.second.dy_mm}
        };
    }
    return j.dump();
}

Table from_json(std::string_view text) {
    Table t;
    auto j = json::parse(text, nullptr, false);
    if (!j.is_object()) return t;
    for (auto it = j.begin(); it != j.end(); ++it) {
        const auto & v = it.value();
        Override o;
        o.rotation_delta_deg = v.value("rot_delta", 0.0);
        o.flip_side          = v.value("flip",      false);
        o.dx_mm              = v.value("dx",        0.0);
        o.dy_mm              = v.value("dy",        0.0);
        t[it.key()] = o;
    }
    return t;
}

} // namespace pos_overrides
