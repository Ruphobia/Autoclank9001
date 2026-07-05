// SPDX-License-Identifier: GPL-3.0-or-later
#include "fab_export.hpp"

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

namespace fab_export {

using kicad_model::Board;
using kicad_model::Footprint;
using kicad_model::GrLine;
using kicad_model::NetInfo;
using kicad_model::ItemType;

namespace {

std::string mm(double v) {
    std::ostringstream os; os.precision(4); os << std::fixed << v;
    std::string s = os.str();
    while (!s.empty() && s.back() == '0') s.pop_back();
    if (!s.empty() && s.back() == '.') s.pop_back();
    return s;
}

std::vector<std::pair<geom::VECTOR2I, geom::VECTOR2I>> edge_cuts(const Board & b) {
    std::vector<std::pair<geom::VECTOR2I, geom::VECTOR2I>> out;
    for (const auto & it : b.items) {
        if (it->type != ItemType::PcbGrLine) continue;
        const auto * g = static_cast<const GrLine*>(it.get());
        if (g->layer == "Edge.Cuts") out.emplace_back(g->start, g->end);
    }
    return out;
}

} // namespace

// ==================== ODB++ ====================

Bundle write_odbpp(const Board & board, std::string_view job_name) {
    Bundle out;
    std::string jn(job_name);

    // misc/info
    std::ostringstream info;
    info << "JOB_NAME    " << jn << "\n"
         << "PRODUCT_MODEL AC9\n"
         << "PRODUCT_MODEL_ID 1\n"
         << "UNITS       MM\n"
         << "CREATION_DATE generated\n"
         << "PRODUCT     ac9/fab_export\n";
    out.files[jn + "/misc/info"] = info.str();

    // misc/attrlist
    out.files[jn + "/misc/attrlist"] = "# ODB++ attrlist (empty)\n";

    // matrix/matrix (layer stack)
    std::ostringstream mat;
    mat << "MATRIX {\n";
    int col = 1;
    for (const auto & L : board.layers) {
        if (L.type != "signal" && L.type != "user") continue;
        mat << "  STEP {\n"
            << "    COL " << col++ << "\n"
            << "    NAME " << L.canonical_name << "\n"
            << "    TYPE " << (L.type == "signal" ? "SIGNAL" : "DOCUMENT") << "\n"
            << "  }\n";
    }
    mat << "}\n";
    out.files[jn + "/matrix/matrix"] = mat.str();

    // steps/pcb/stephdr
    std::ostringstream hdr;
    hdr << "STEP-HDR\n"
        << "X_DATUM=0\nY_DATUM=0\nID=1\n"
        << "X_ORIGIN=0\nY_ORIGIN=0\n";
    out.files[jn + "/steps/pcb/stephdr"] = hdr.str();

    // steps/pcb/profile: board outline as feature file.
    std::ostringstream pro;
    pro << "# Board outline (Edge.Cuts) as PROFILE\n";
    for (const auto & e : edge_cuts(board)) {
        pro << "L " << mm(geom::nm_to_mm(e.first.x))
            << " " << mm(geom::nm_to_mm(e.first.y))
            << " " << mm(geom::nm_to_mm(e.second.x))
            << " " << mm(geom::nm_to_mm(e.second.y))
            << " 0.1\n";
    }
    out.files[jn + "/steps/pcb/profile"] = pro.str();

    // steps/pcb/eda/data: nets + components.
    std::ostringstream eda;
    eda << "H optimize 1\n";
    for (const auto & n : board.nets) {
        if (n.id == 0) continue;
        eda << "NET " << n.id << " " << n.name << "\n";
    }
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint*>(it.get());
        std::string ref;
        for (const auto & f : fp->fields) if (f.name == "Reference") { ref = f.value; break; }
        eda << "COMP " << (ref.empty() ? fp->lib_id : ref)
            << " X=" << mm(geom::nm_to_mm(fp->at.x))
            << " Y=" << mm(geom::nm_to_mm(fp->at.y))
            << " ROT=" << fp->angle.deg()
            << " SIDE=" << (fp->placement_layer == "B.Cu" ? "B" : "T")
            << "\n";
    }
    out.files[jn + "/steps/pcb/eda/data"] = eda.str();

    return out;
}

// ==================== HyperLynx HYP ====================

std::string write_hyperlynx(const Board & board) {
    std::ostringstream os;
    os << "{VERSION=2.14}\n"
       << "{UNITS=METRIC LENGTH}\n"
       << "{BOARD\n";
    for (const auto & e : edge_cuts(board)) {
        os << "  (PERIMETER_SEGMENT X1=" << mm(geom::nm_to_mm(e.first.x))
           << " Y1=" << mm(geom::nm_to_mm(e.first.y))
           << " X2=" << mm(geom::nm_to_mm(e.second.x))
           << " Y2=" << mm(geom::nm_to_mm(e.second.y))
           << ")\n";
    }
    os << "}\n"
       << "{STACKUP\n"
       << "  (SIGNAL T=0.035 P=Copper L=F.Cu)\n"
       << "  (DIELECTRIC T=1.53 P=FR-4 C=4.4)\n"
       << "  (SIGNAL T=0.035 P=Copper L=B.Cu)\n"
       << "}\n"
       << "{NET_DEFN\n";
    for (const auto & n : board.nets) {
        if (n.id == 0) continue;
        os << "  (NET " << n.name << ")\n";
    }
    os << "}\n"
       << "{DEVICES\n";
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint*>(it.get());
        std::string ref;
        for (const auto & f : fp->fields) if (f.name == "Reference") { ref = f.value; break; }
        os << "  ? REF=" << (ref.empty() ? fp->lib_id : ref)
           << " NAME=" << fp->lib_id
           << " LAYER=" << (fp->placement_layer == "B.Cu" ? "B.Cu" : "F.Cu")
           << " ROT=" << fp->angle.deg()
           << " X=" << mm(geom::nm_to_mm(fp->at.x))
           << " Y=" << mm(geom::nm_to_mm(fp->at.y))
           << "\n";
    }
    os << "}\n"
       << "{END}\n";
    return os.str();
}

// ==================== IDF v3 ====================

IdfBundle write_idf(const Board & board) {
    IdfBundle out;
    std::ostringstream emn, emp;

    emn << ".HEADER\n"
        << "BOARD_FILE  3.0  \"ac9/fab_export\"  generated 1\n"
        << "ac9_board  MM\n"
        << ".END_HEADER\n"
        << ".BOARD_OUTLINE  MCAD\n"
        << "1.6\n";
    int loop = 0;
    for (const auto & e : edge_cuts(board)) {
        emn << loop << " "
            << mm(geom::nm_to_mm(e.first.x))  << " "
            << mm(geom::nm_to_mm(e.first.y))  << " 0\n"
            << loop << " "
            << mm(geom::nm_to_mm(e.second.x)) << " "
            << mm(geom::nm_to_mm(e.second.y)) << " 0\n";
    }
    emn << ".END_BOARD_OUTLINE\n"
        << ".PLACEMENT\n";

    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint*>(it.get());
        std::string ref;
        for (const auto & f : fp->fields) if (f.name == "Reference") { ref = f.value; break; }
        std::string comp_name = fp->lib_id;
        // Sanitize for IDF token rules.
        for (auto & c : comp_name) if (c == ' ' || c == ':') c = '_';
        emn << comp_name << " " << (ref.empty() ? std::string("?") : ref)
            << " PLACED\n"
            << mm(geom::nm_to_mm(fp->at.x)) << " "
            << mm(geom::nm_to_mm(fp->at.y)) << " 0 "
            << fp->angle.deg() << " "
            << (fp->placement_layer == "B.Cu" ? "BOTTOM" : "TOP") << " MCAD\n";
    }
    emn << ".END_PLACEMENT\n";

    emp << ".HEADER\n"
        << "LIBRARY_FILE  3.0  \"ac9/fab_export\"  generated 1\n"
        << ".END_HEADER\n";
    // One placeholder component per unique lib_id.
    std::vector<std::string> emitted;
    for (const auto & it : board.items) {
        if (it->type != ItemType::PcbFootprint) continue;
        const auto * fp = static_cast<const Footprint*>(it.get());
        std::string cn = fp->lib_id;
        for (auto & c : cn) if (c == ' ' || c == ':') c = '_';
        bool seen = false;
        for (const auto & e : emitted) if (e == cn) { seen = true; break; }
        if (seen) continue;
        emitted.push_back(cn);
        emp << ".ELECTRICAL\n"
            << cn << " " << cn << " MM 1.5\n"
            << "0 -1.5 -1.5 0\n"
            << "0 1.5 -1.5 0\n"
            << "0 1.5 1.5 0\n"
            << "0 -1.5 1.5 0\n"
            << "0 -1.5 -1.5 0\n"
            << ".END_ELECTRICAL\n";
    }

    out.emn = emn.str();
    out.emp = emp.str();
    return out;
}

} // namespace fab_export
