#include "hex_editor.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace hex_editor {
namespace {

const char kHex[] = "0123456789abcdef";

template <typename T>
bool read_le(const uint8_t * d, size_t n, size_t off, T & out, bool big_endian) {
    if (off + sizeof(T) > n) return false;
    if (!big_endian) {
        std::memcpy(&out, d + off, sizeof(T));
        return true;
    }
    uint8_t buf[sizeof(T)];
    for (size_t i = 0; i < sizeof(T); ++i) buf[i] = d[off + sizeof(T) - 1 - i];
    std::memcpy(&out, buf, sizeof(T));
    return true;
}

}

std::string dump(const uint8_t * data, size_t size, uint64_t base, size_t per_line) {
    if (per_line == 0) per_line = 16;
    std::string out;
    char        line_buf[80];
    for (size_t off = 0; off < size; off += per_line) {
        const size_t take = std::min(per_line, size - off);
        std::snprintf(line_buf, sizeof(line_buf), "%08llx  ",
                      static_cast<unsigned long long>(base + off));
        out += line_buf;

        for (size_t i = 0; i < per_line; ++i) {
            if (i < take) {
                uint8_t b = data[off + i];
                out.push_back(kHex[b >> 4]);
                out.push_back(kHex[b & 0xF]);
            } else {
                out.append("  ");
            }
            out.push_back(i + 1 == per_line / 2 ? ' ' : ' ');
        }
        out.append(" |");
        for (size_t i = 0; i < take; ++i) {
            uint8_t b = data[off + i];
            out.push_back((b >= 0x20 && b < 0x7F) ? static_cast<char>(b) : '.');
        }
        out.append("|\n");
    }
    return out;
}

bool read_u8 (const uint8_t * d, size_t n, size_t o, uint8_t  & v)                       { return read_le<uint8_t> (d, n, o, v, false);     }
bool read_u16(const uint8_t * d, size_t n, size_t o, uint16_t & v, bool be)              { return read_le<uint16_t>(d, n, o, v, be);        }
bool read_u32(const uint8_t * d, size_t n, size_t o, uint32_t & v, bool be)              { return read_le<uint32_t>(d, n, o, v, be);        }
bool read_u64(const uint8_t * d, size_t n, size_t o, uint64_t & v, bool be)              { return read_le<uint64_t>(d, n, o, v, be);        }
bool read_i8 (const uint8_t * d, size_t n, size_t o, int8_t   & v)                       { return read_le<int8_t>  (d, n, o, v, false);     }
bool read_i16(const uint8_t * d, size_t n, size_t o, int16_t  & v, bool be)              { return read_le<int16_t> (d, n, o, v, be);        }
bool read_i32(const uint8_t * d, size_t n, size_t o, int32_t  & v, bool be)              { return read_le<int32_t> (d, n, o, v, be);        }
bool read_i64(const uint8_t * d, size_t n, size_t o, int64_t  & v, bool be)              { return read_le<int64_t> (d, n, o, v, be);        }
bool read_f32(const uint8_t * d, size_t n, size_t o, float    & v, bool be)              { return read_le<float>   (d, n, o, v, be);        }
bool read_f64(const uint8_t * d, size_t n, size_t o, double   & v, bool be)              { return read_le<double>  (d, n, o, v, be);        }

FormatId detect_format(const uint8_t * d, size_t n) {
    auto starts = [&](std::initializer_list<uint8_t> sig) -> bool {
        if (n < sig.size()) return false;
        size_t i = 0;
        for (uint8_t b : sig) { if (d[i++] != b) return false; }
        return true;
    };

    if (starts({0x7F, 'E', 'L', 'F'}))                          return {"ELF",  "ELF executable / shared object"};
    if (starts({0x89, 'P',  'N',  'G', 0x0D, 0x0A, 0x1A, 0x0A})) return {"PNG",  "PNG image"};
    if (starts({0xFF, 0xD8, 0xFF}))                              return {"JPEG", "JFIF / Exif image"};
    if (starts({'G',  'I',  'F',  '8'}))                          return {"GIF",  "GIF image"};
    if (starts({'G',  'G',  'U',  'F'}))                          return {"GGUF", "llama.cpp model"};
    if (starts({'P',  'K',  0x03, 0x04}))                         return {"ZIP",  "ZIP archive (or ZIP-based container)"};
    if (starts({0x1F, 0x8B}))                                     return {"GZIP", "gzip stream"};
    if (starts({'B',  'M'}))                                      return {"BMP",  "BMP image"};
    if (starts({'%',  'P',  'D',  'F'}))                          return {"PDF",  "PDF document"};
    if (n >= 0x40 && d[0] == 'M' && d[1] == 'Z') {
        uint32_t pe_off = 0;
        if (read_u32(d, n, 0x3C, pe_off) && pe_off + 4 <= n &&
            d[pe_off] == 'P' && d[pe_off+1] == 'E' && d[pe_off+2] == 0 && d[pe_off+3] == 0) {
            return {"PE", "Windows PE / COFF binary"};
        }
        return {"DOS-MZ", "DOS executable (no PE header)"};
    }
    if (n > 262 && std::memcmp(d + 257, "ustar", 5) == 0)
        return {"TAR", "POSIX ustar archive"};

    return {"unknown", ""};
}

double shannon_entropy(const uint8_t * d, size_t n) {
    if (n == 0) return 0.0;
    size_t counts[256] = {0};
    for (size_t i = 0; i < n; ++i) ++counts[d[i]];
    const double inv = 1.0 / static_cast<double>(n);
    double h = 0.0;
    for (size_t i = 0; i < 256; ++i) {
        if (!counts[i]) continue;
        double p = counts[i] * inv;
        h -= p * (std::log(p) / std::log(2.0));
    }
    return h;
}

uint32_t crc32(const uint8_t * d, size_t n) {
    static uint32_t tab[256];
    static bool     ready = false;
    if (!ready) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int k = 0; k < 8; ++k)
                c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            tab[i] = c;
        }
        ready = true;
    }
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < n; ++i) c = tab[(c ^ d[i]) & 0xFF] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

std::vector<DiffSegment> diff(const uint8_t * a, size_t sa,
                              const uint8_t * b, size_t sb,
                              size_t coalesce_gap) {
    std::vector<DiffSegment> out;
    const size_t common = std::min(sa, sb);

    DiffSegment cur;
    bool        in_run    = false;
    size_t      gap_count = 0;

    for (size_t i = 0; i < common; ++i) {
        if (a[i] != b[i]) {
            if (!in_run) { cur = {}; cur.offset = i; in_run = true; gap_count = 0; }
            // Close any pending gap into the current run.
            for (size_t g = 0; g < gap_count; ++g) {
                cur.a.push_back(a[i - gap_count + g - 0]);
                cur.b.push_back(b[i - gap_count + g - 0]);
            }
            gap_count = 0;
            cur.a.push_back(a[i]);
            cur.b.push_back(b[i]);
        } else if (in_run) {
            ++gap_count;
            if (gap_count > coalesce_gap) {
                out.push_back(std::move(cur));
                in_run    = false;
                gap_count = 0;
            }
        }
    }
    if (in_run) out.push_back(std::move(cur));

    if (sa != sb) {
        DiffSegment tail;
        tail.offset = common;
        for (size_t i = common; i < sa; ++i) tail.a.push_back(a[i]);
        for (size_t i = common; i < sb; ++i) tail.b.push_back(b[i]);
        out.push_back(std::move(tail));
    }
    return out;
}

void apply_patches(std::string_view              path,
                   const std::vector<Patch>    & patches,
                   bool                          make_backup) {
    namespace fs = std::filesystem;
    fs::path p(std::string{path});
    if (!fs::exists(p)) throw std::runtime_error("hex_editor::apply_patches: no such file: " + p.string());
    const uintmax_t size = fs::file_size(p);

    for (const auto & q : patches) {
        if (q.offset + q.bytes.size() > size)
            throw std::runtime_error("hex_editor::apply_patches: patch extends past EOF");
    }

    if (make_backup) {
        fs::path bak = p; bak += ".bak";
        std::error_code ec;
        fs::copy_file(p, bak, fs::copy_options::overwrite_existing, ec);
        if (ec) throw std::runtime_error("hex_editor::apply_patches: backup failed: " + ec.message());
    }

    std::fstream f(p, std::ios::in | std::ios::out | std::ios::binary);
    if (!f) throw std::runtime_error("hex_editor::apply_patches: cannot open for write: " + p.string());
    for (const auto & q : patches) {
        f.seekp(static_cast<std::streamoff>(q.offset), std::ios::beg);
        f.write(reinterpret_cast<const char *>(q.bytes.data()),
                static_cast<std::streamsize>(q.bytes.size()));
        if (!f) throw std::runtime_error("hex_editor::apply_patches: write failed");
    }
    f.flush();
}

}
