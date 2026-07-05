// SPDX-License-Identifier: GPL-3.0-or-later
#include "mesh_load.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace mesh_load {

namespace {

void update_bounds(Mesh & m, const Vertex & v) {
    if (m.vertices.size() == 1) {
        m.min_x = m.max_x = v.x;
        m.min_y = m.max_y = v.y;
        m.min_z = m.max_z = v.z;
    } else {
        if (v.x < m.min_x) m.min_x = v.x; else if (v.x > m.max_x) m.max_x = v.x;
        if (v.y < m.min_y) m.min_y = v.y; else if (v.y > m.max_y) m.max_y = v.y;
        if (v.z < m.min_z) m.min_z = v.z; else if (v.z > m.max_z) m.max_z = v.z;
    }
}

std::string ext_lower(std::string_view s) {
    std::string o(s);
    std::transform(o.begin(), o.end(), o.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return o;
}

} // namespace

std::optional<Mesh> load_wrl(std::string_view text) {
    Mesh m;
    // Look for coord Coordinate { point [ ... ] } and coordIndex [ ... ].
    auto pos_pt   = text.find("point [");
    auto pos_ci   = text.find("coordIndex [");
    if (pos_pt == std::string_view::npos) return std::nullopt;

    std::size_t s = pos_pt + std::strlen("point [");
    auto e = text.find(']', s);
    if (e == std::string_view::npos) return std::nullopt;
    std::string points(text.substr(s, e - s));

    // Point stream: "x y z, x y z, ..."
    char * p = points.data();
    char * endp;
    while (*p) {
        Vertex v;
        v.x = std::strtof(p, &endp); if (endp == p) break; p = endp;
        v.y = std::strtof(p, &endp); if (endp == p) break; p = endp;
        v.z = std::strtof(p, &endp); if (endp == p) break; p = endp;
        m.vertices.push_back(v);
        update_bounds(m, v);
        // Skip commas etc.
        while (*p && (*p == ',' || std::isspace(static_cast<unsigned char>(*p)))) ++p;
    }

    if (pos_ci != std::string_view::npos) {
        std::size_t si = pos_ci + std::strlen("coordIndex [");
        auto ei = text.find(']', si);
        if (ei != std::string_view::npos) {
            std::string idx(text.substr(si, ei - si));
            char * ip = idx.data();
            char * iendp;
            std::vector<int> face;
            while (*ip) {
                long n = std::strtol(ip, &iendp, 10);
                if (iendp == ip) break;
                ip = iendp;
                while (*ip && (*ip == ',' || std::isspace(static_cast<unsigned char>(*ip)))) ++ip;
                if (n < 0) {
                    // Triangulate face as a fan.
                    if (face.size() >= 3) {
                        for (std::size_t i = 1; i + 1 < face.size(); ++i) {
                            m.indices.push_back(face[0]);
                            m.indices.push_back(face[i]);
                            m.indices.push_back(face[i + 1]);
                        }
                    }
                    face.clear();
                } else {
                    face.push_back(static_cast<int>(n));
                }
            }
        }
    } else {
        // If no coordIndex, emit sequential triangles.
        for (std::size_t i = 0; i + 2 < m.vertices.size(); i += 3) {
            m.indices.push_back(i);
            m.indices.push_back(i + 1);
            m.indices.push_back(i + 2);
        }
    }
    return m;
}

std::optional<Mesh> load_stl(std::string_view data) {
    // Binary STL detection: 80-byte header + uint32 facet count.
    if (data.size() >= 84 && data.substr(0, 5) != "solid") {
        Mesh m;
        std::uint32_t n;
        std::memcpy(&n, data.data() + 80, 4);
        std::size_t p = 84;
        for (std::uint32_t i = 0; i < n && p + 50 <= data.size(); ++i, p += 50) {
            // 12 bytes normal + 3*12 bytes vertex + 2 bytes attr count.
            for (int v = 0; v < 3; ++v) {
                float xyz[3];
                std::memcpy(xyz, data.data() + p + 12 + v * 12, 12);
                Vertex vx{ xyz[0], xyz[1], xyz[2] };
                m.indices.push_back(static_cast<std::uint32_t>(m.vertices.size()));
                m.vertices.push_back(vx);
                update_bounds(m, vx);
            }
        }
        if (!m.vertices.empty()) return m;
    }
    // ASCII STL.
    Mesh m;
    std::size_t i = 0;
    while (i < data.size()) {
        auto v = data.find("vertex", i);
        if (v == std::string_view::npos) break;
        v += 6;
        Vertex vx;
        vx.x = std::strtof(data.data() + v, nullptr);
        std::size_t j = data.find(' ', v);
        if (j == std::string_view::npos) break;
        vx.y = std::strtof(data.data() + j + 1, nullptr);
        std::size_t k = data.find(' ', j + 1);
        if (k == std::string_view::npos) break;
        vx.z = std::strtof(data.data() + k + 1, nullptr);
        m.indices.push_back(static_cast<std::uint32_t>(m.vertices.size()));
        m.vertices.push_back(vx);
        update_bounds(m, vx);
        i = data.find('\n', k);
        if (i == std::string_view::npos) break;
        ++i;
    }
    return m.vertices.empty() ? std::nullopt : std::optional<Mesh>(m);
}

std::optional<Mesh> load_step(std::string_view text) {
    Mesh m;
    // Extract every CARTESIAN_POINT('',(x,y,z));
    std::size_t p = 0;
    while ((p = text.find("CARTESIAN_POINT", p)) != std::string_view::npos) {
        auto lp = text.find("((", p);
        if (lp == std::string_view::npos) break;
        auto rp = text.find(")", lp + 2);
        if (rp == std::string_view::npos) break;
        std::string inside(text.substr(lp + 2, rp - lp - 2));
        Vertex v;
        char * ptr = inside.data();
        v.x = std::strtof(ptr, &ptr);
        if (*ptr == ',' || *ptr == ' ') ++ptr;
        v.y = std::strtof(ptr, &ptr);
        if (*ptr == ',' || *ptr == ' ') ++ptr;
        v.z = std::strtof(ptr, &ptr);
        m.vertices.push_back(v);
        update_bounds(m, v);
        p = rp;
    }
    // Trivial triangulation: chunks of 3 points -> triangle. For our
    // own STEP writer output that yields recognizable box wireframes.
    for (std::size_t i = 0; i + 2 < m.vertices.size(); i += 3) {
        m.indices.push_back(i);
        m.indices.push_back(i + 1);
        m.indices.push_back(i + 2);
    }
    return m.vertices.empty() ? std::nullopt : std::optional<Mesh>(m);
}

std::optional<Mesh> load(std::string_view data, std::string_view ext) {
    std::string e = ext_lower(ext);
    if (e == "wrl" || e == "vrml" || e == "x3d") return load_wrl(data);
    if (e == "stl")                              return load_stl(data);
    if (e == "step" || e == "stp")               return load_step(data);
    return std::nullopt;
}

std::string to_json(const Mesh & m) {
    std::ostringstream os;
    os << "{\"vertices\":[";
    for (std::size_t i = 0; i < m.vertices.size(); ++i) {
        if (i) os << ',';
        os << m.vertices[i].x << ',' << m.vertices[i].y << ',' << m.vertices[i].z;
    }
    os << "],\"indices\":[";
    for (std::size_t i = 0; i < m.indices.size(); ++i) {
        if (i) os << ',';
        os << m.indices[i];
    }
    os << "],\"bbox\":{\"lo\":[" << m.min_x << ',' << m.min_y << ',' << m.min_z
       << "],\"hi\":[" << m.max_x << ',' << m.max_y << ',' << m.max_z << "]}}";
    return os.str();
}

} // namespace mesh_load
