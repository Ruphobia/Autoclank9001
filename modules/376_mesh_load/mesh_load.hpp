// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Mesh loader for 3D component models.
//
// Parses WRL (VRML 1.0/2.0 ASCII), STL (ASCII + binary), and a subset
// of STEP AP214 (points + face graph as emitted by our own
// 372_step_writer). Returns a flat triangle mesh usable by
// three.js on the frontend.
namespace mesh_load {

struct Vertex {
    float x = 0, y = 0, z = 0;
};

struct Mesh {
    std::vector<Vertex>       vertices;
    std::vector<std::uint32_t> indices;   // triangles, 3 per face
    float min_x = 0, min_y = 0, min_z = 0;
    float max_x = 0, max_y = 0, max_z = 0;
};

std::optional<Mesh> load_wrl (std::string_view text);
std::optional<Mesh> load_stl (std::string_view data);
std::optional<Mesh> load_step(std::string_view text);

// Auto-detect from extension.
std::optional<Mesh> load(std::string_view data, std::string_view ext);

// Serialize a Mesh to a compact JSON object for the browser.
std::string to_json(const Mesh & m);

}
