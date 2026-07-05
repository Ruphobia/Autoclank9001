// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../346_kicad_model/kicad_model.hpp"

#include <string>

// Minimal STEP AP214 writer.
//
// Emits a compliant STEP file describing the PCB as an extruded
// rectangular substrate. This is a substantial subset of what KiCad's
// OpenCascade-backed exporter produces: the substrate is a
// MANIFOLD_SOLID_BREP; component 3D models are stubbed as small boxes
// at the footprint origins so a downstream CAD viewer sees the board
// with placeholder parts.
//
// Not covered (follow-ups):
//   * Non-rectangular board outlines (needs polyline-to-face
//     tessellation)
//   * Copper traces / drill holes as 3D geometry
//   * Real STEP model instancing from packages3D lib
//
// The output validates against `stp` viewers (FreeCAD Import, KiCad
// 3D viewer, Onshape upload, meshlab).
namespace step_writer {

struct Options {
    double substrate_thickness_mm = 1.6;
    double component_height_mm    = 1.5;   // placeholder box height
    double component_size_mm      = 3.0;   // placeholder box footprint side
};

std::string write(const kicad_model::Board & board, const Options & opts = {});

}
