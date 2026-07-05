// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/376_mesh_load/mesh_load.hpp"

namespace {

testing::TestOutcome run() {
    // WRL.
    const char * wrl =
        "#VRML V2.0 utf8\n"
        "Shape { geometry IndexedFaceSet {\n"
        "  coord Coordinate { point [\n"
        "    0 0 0, 1 0 0, 1 1 0, 0 1 0\n"
        "  ] }\n"
        "  coordIndex [ 0 1 2 -1  0 2 3 -1 ]\n"
        "} }\n";
    auto m = mesh_load::load_wrl(wrl);
    if (!m)                        return testing::fail("wrl parse");
    if (m->vertices.size() != 4)   return testing::fail("wrl verts");
    if (m->indices.size()  != 6)   return testing::fail("wrl tri count");
    if (m->max_x <= m->min_x)      return testing::fail("wrl bbox");

    // ASCII STL.
    const char * stl =
        "solid t\n"
        "facet normal 0 0 1\n"
        " outer loop\n"
        "  vertex 0 0 0\n"
        "  vertex 1 0 0\n"
        "  vertex 0 1 0\n"
        " endloop\n"
        "endfacet\n"
        "endsolid t\n";
    auto ms = mesh_load::load_stl(stl);
    if (!ms || ms->vertices.size() != 3) return testing::fail("stl");

    // STEP: our own writer's output.
    const char * step =
        "ISO-10303-21;\nHEADER;\nENDSEC;\nDATA;\n"
        "#1 = CARTESIAN_POINT('',(0.0,0.0,0.0));\n"
        "#2 = CARTESIAN_POINT('',(1.0,0.0,0.0));\n"
        "#3 = CARTESIAN_POINT('',(0.0,1.0,0.0));\n"
        "ENDSEC;\nEND-ISO-10303-21;\n";
    auto ms2 = mesh_load::load_step(step);
    if (!ms2 || ms2->vertices.size() != 3) return testing::fail("step");

    // JSON round-trip.
    std::string j = mesh_load::to_json(*m);
    if (j.find("\"vertices\"") == std::string::npos) return testing::fail("json vertices");
    if (j.find("\"indices\"")  == std::string::npos) return testing::fail("json indices");
    if (j.find("\"bbox\"")     == std::string::npos) return testing::fail("json bbox");

    return testing::ok();
}

const int _r = testing::register_test(
    "mesh_load",
    "Mesh importer: VRML with coordIndex fan-triangulation, ASCII STL vertex extraction, STEP CARTESIAN_POINT scan.",
    &run);

} // namespace
