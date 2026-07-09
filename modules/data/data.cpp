// SPDX-License-Identifier: GPL-3.0-or-later
#include "data.hpp"
#include "../model_chunks.hpp"

#include <openssl/evp.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

namespace data {
namespace fs = std::filesystem;
using json    = nlohmann::json;

namespace {

constexpr std::size_t kIOBuf = 4 * 1024 * 1024;   // 4 MiB streaming buffer

// Convert a raw digest to lowercase hex.
std::string hex(const unsigned char * d, unsigned int n) {
    static const char lut[] = "0123456789abcdef";
    std::string s;
    s.resize(n * 2);
    for (unsigned int i = 0; i < n; ++i) {
        s[2 * i]     = lut[(d[i] >> 4) & 0x0F];
        s[2 * i + 1] = lut[d[i] & 0x0F];
    }
    return s;
}

// RAII wrapper so we never leak an EVP_MD_CTX on early return / throw.
struct MdCtx {
    EVP_MD_CTX * ctx = EVP_MD_CTX_new();
    MdCtx() {
        if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");
        if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
            EVP_MD_CTX_free(ctx);
            throw std::runtime_error("EVP_DigestInit_ex sha256 failed");
        }
    }
    ~MdCtx() { if (ctx) EVP_MD_CTX_free(ctx); }
    MdCtx(const MdCtx &)             = delete;
    MdCtx & operator=(const MdCtx &) = delete;

    void update(const void * data, std::size_t n) {
        if (EVP_DigestUpdate(ctx, data, n) != 1)
            throw std::runtime_error("EVP_DigestUpdate failed");
    }
    std::string hex_final() {
        unsigned char buf[EVP_MAX_MD_SIZE];
        unsigned int  len = 0;
        if (EVP_DigestFinal_ex(ctx, buf, &len) != 1)
            throw std::runtime_error("EVP_DigestFinal_ex failed");
        return hex(buf, len);
    }
};

// Human-friendly byte size for progress printing.
std::string human_bytes(std::uintmax_t n) {
    char b[64];
    if (n >= (1ULL << 30))
        std::snprintf(b, sizeof(b), "%.2f GB",
                      static_cast<double>(n) / (1ULL << 30));
    else if (n >= (1ULL << 20))
        std::snprintf(b, sizeof(b), "%.1f MB",
                      static_cast<double>(n) / (1ULL << 20));
    else
        std::snprintf(b, sizeof(b), "%ju B",
                      static_cast<std::uintmax_t>(n));
    return b;
}

}  // namespace

std::string sha256_file(const fs::path & p) {
    std::ifstream f(p, std::ios::binary);
    if (!f) {
        throw std::runtime_error("sha256_file: cannot open " + p.string());
    }
    MdCtx mc;
    std::vector<char> buf(kIOBuf);
    while (f) {
        f.read(buf.data(), buf.size());
        auto n = static_cast<std::size_t>(f.gcount());
        if (n > 0) mc.update(buf.data(), n);
    }
    if (f.bad()) {
        throw std::runtime_error("sha256_file: read error on " + p.string());
    }
    return mc.hex_final();
}

ChunkResult chunk_asset(const fs::path & input,
                        const fs::path & data_dir,
                        std::uintmax_t   max_chunk_size) {
    if (!fs::exists(input)) {
        throw std::runtime_error("chunk_asset: input does not exist: " +
                                 input.string());
    }
    std::error_code ec;
    fs::create_directories(data_dir, ec);
    if (ec) {
        throw std::runtime_error("chunk_asset: cannot create " +
                                 data_dir.string() + ": " + ec.message());
    }

    ChunkResult r;
    r.size_bytes = fs::file_size(input);

    std::fprintf(stderr,
        "data: chunking %s (%s) into pieces of %s\n",
        input.filename().string().c_str(),
        human_bytes(r.size_bytes).c_str(),
        human_bytes(max_chunk_size).c_str());

    std::ifstream f(input, std::ios::binary);
    if (!f) {
        throw std::runtime_error("chunk_asset: cannot open " + input.string());
    }
    MdCtx full_ctx;

    std::vector<char> buf(kIOBuf);
    std::uintmax_t    chunk_idx    = 0;
    std::uintmax_t    total_done   = 0;

    while (f && !f.eof()) {
        // Stream up to max_chunk_size into a temp file, hashing both
        // the whole file and this chunk in parallel.
        MdCtx         chunk_ctx;
        fs::path      tmp = data_dir / (".chunk_" +
                                         std::to_string(chunk_idx) + ".tmp");
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out) {
            throw std::runtime_error("chunk_asset: cannot create tmp " +
                                     tmp.string());
        }
        std::uintmax_t written = 0;
        while (written < max_chunk_size && f) {
            std::size_t want = static_cast<std::size_t>(
                std::min<std::uintmax_t>(buf.size(),
                                         max_chunk_size - written));
            f.read(buf.data(), static_cast<std::streamsize>(want));
            auto got = static_cast<std::size_t>(f.gcount());
            if (got == 0) break;
            out.write(buf.data(), static_cast<std::streamsize>(got));
            if (!out) {
                throw std::runtime_error("chunk_asset: write error on " +
                                         tmp.string());
            }
            chunk_ctx.update(buf.data(), got);
            full_ctx.update(buf.data(), got);
            written    += got;
            total_done += got;
        }
        out.close();

        if (written == 0) {
            // Nothing more to read; drop the empty tmp and stop.
            fs::remove(tmp, ec);
            break;
        }

        const std::string sha  = chunk_ctx.hex_final();
        const fs::path    dest = data_dir / (sha + ".bin");
        if (fs::exists(dest)) {
            // Dedup: identical chunk already on disk.
            fs::remove(tmp, ec);
        } else {
            fs::rename(tmp, dest, ec);
            if (ec) {
                throw std::runtime_error("chunk_asset: rename failed " +
                                         tmp.string() + " -> " +
                                         dest.string() + ": " + ec.message());
            }
        }
        r.chunk_shas.push_back(sha);
        ++chunk_idx;
        std::fprintf(stderr,
            "  chunk %ju: %s  (%s / %s)\n",
            chunk_idx, sha.c_str(),
            human_bytes(total_done).c_str(),
            human_bytes(r.size_bytes).c_str());
        std::fflush(stderr);
    }
    if (f.bad()) {
        throw std::runtime_error("chunk_asset: read error on " +
                                 input.string());
    }
    r.full_sha256 = full_ctx.hex_final();
    return r;
}

void set_role_in_manifest(const fs::path      & data_dir,
                          const std::string   & role,
                          const std::string   & human_name,
                          const ChunkResult   & result) {
    const fs::path mpath = data_dir / "manifest.json";
    // Serialize on flock(mpath) so parallel writers can't race.
    int fd = ::open(mpath.c_str(), O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        throw std::runtime_error("set_role_in_manifest: cannot open " +
                                 mpath.string() + ": " + std::strerror(errno));
    }
    if (::flock(fd, LOCK_EX) != 0) {
        int e = errno; ::close(fd);
        throw std::runtime_error("set_role_in_manifest: flock failed: " +
                                 std::string(std::strerror(e)));
    }
    // Read current contents.
    off_t sz = ::lseek(fd, 0, SEEK_END);
    ::lseek(fd, 0, SEEK_SET);
    json m = json::object();
    if (sz > 0) {
        std::string cur(static_cast<std::size_t>(sz), '\0');
        ssize_t nr = ::read(fd, cur.data(), cur.size());
        if (nr > 0) {
            try { m = json::parse(cur); }
            catch (...) { m = json::object(); }
        }
    }
    // Merge role entry.
    json entry;
    entry["human_name"]  = human_name;
    entry["sha256"]      = result.full_sha256;
    entry["size_bytes"]  = result.size_bytes;
    entry["chunk_count"] = result.chunk_shas.size();
    entry["chunks"]      = result.chunk_shas;
    m[role] = entry;

    // Rewrite atomically via tmp + rename to avoid a partial file if we crash.
    const fs::path tmp = data_dir / "manifest.json.tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        out << m.dump(2) << "\n";
    }
    std::error_code ec;
    fs::rename(tmp, mpath, ec);
    ::flock(fd, LOCK_UN);
    ::close(fd);
    if (ec) {
        throw std::runtime_error("set_role_in_manifest: rename failed: " +
                                 ec.message());
    }
}

fs::path resolve_role(const std::string & role, const fs::path & data_dir) {
    const fs::path mpath = data_dir / "manifest.json";
    std::ifstream mf(mpath);
    if (!mf) {
        throw std::runtime_error("resolve_role: no manifest at " + mpath.string());
    }
    json manifest;
    try { mf >> manifest; }
    catch (const std::exception & ex) {
        throw std::runtime_error("resolve_role: bad manifest json: " +
                                 std::string(ex.what()));
    }
    if (!manifest.contains(role)) {
        throw std::runtime_error("resolve_role: role \"" + role +
                                 "\" not in manifest");
    }
    const auto & entry = manifest[role];
    const std::string full_sha  = entry.value("sha256", std::string{});
    const std::string human     = entry.value("human_name", role);
    const auto        chunks    = entry.value("chunks", std::vector<std::string>{});
    if (full_sha.empty() || chunks.empty()) {
        throw std::runtime_error("resolve_role: role \"" + role +
                                 "\" manifest entry is malformed");
    }

    // Per-role mutex so parallel resolvers of the same role serialize
    // without blocking others.
    static std::mutex                                s_map_mu;
    static std::unordered_map<std::string, std::mutex> s_role_mu;
    std::mutex * role_mu = nullptr;
    {
        std::lock_guard<std::mutex> lk(s_map_mu);
        role_mu = &s_role_mu[role];
    }
    std::lock_guard<std::mutex> lk(*role_mu);

    const fs::path cache_dir = data_dir / "cache";
    std::error_code ec;
    fs::create_directories(cache_dir, ec);

    // File extension guessed from the human_name; falls back to .bin.
    std::string ext = ".bin";
    if (auto dot = human.rfind('.'); dot != std::string::npos) {
        ext = human.substr(dot);
    }
    const fs::path out = cache_dir / (full_sha + ext);

    // Fast path: if cached file exists and matches the manifest sha, use it.
    if (fs::exists(out)) {
        try {
            if (sha256_file(out) == full_sha) return out;
        } catch (...) { /* fall through to rebuild */ }
        fs::remove(out, ec);
    }

    // Reassemble chunks in order into a .tmp, verifying the full-file sha
    // along the way, then rename.
    const fs::path tmp = out.string() + ".tmp";
    std::ofstream o(tmp, std::ios::binary | std::ios::trunc);
    if (!o) {
        throw std::runtime_error("resolve_role: cannot open " + tmp.string());
    }
    unsigned char io[4 * 1024 * 1024];
    // Build a rolling SHA-256 across all chunks so we can verify the final.
    MdCtx full_ctx;
    for (std::size_t idx = 0; idx < chunks.size(); ++idx) {
        const std::string & chunk_sha = chunks[idx];
        const fs::path    cpath     = data_dir / (chunk_sha + ".bin");
        std::ifstream     f(cpath, std::ios::binary);
        if (!f) {
            std::error_code rec;
            fs::remove(tmp, rec);
            throw std::runtime_error("resolve_role: chunk missing: " +
                                     cpath.string());
        }
        // Verify chunk sha as we stream it in.
        MdCtx chunk_ctx;
        while (f) {
            f.read(reinterpret_cast<char *>(io), sizeof(io));
            auto n = static_cast<std::size_t>(f.gcount());
            if (n == 0) break;
            chunk_ctx.update(io, n);
            full_ctx.update(io, n);
            o.write(reinterpret_cast<const char *>(io),
                    static_cast<std::streamsize>(n));
            if (!o) {
                std::error_code rec;
                fs::remove(tmp, rec);
                throw std::runtime_error("resolve_role: write failed on " +
                                         tmp.string());
            }
        }
        const std::string got = chunk_ctx.hex_final();
        if (got != chunk_sha) {
            std::error_code rec;
            fs::remove(tmp, rec);
            throw std::runtime_error("resolve_role: chunk " +
                                     std::to_string(idx) +
                                     " sha mismatch (got " + got +
                                     ", expected " + chunk_sha + ")");
        }
    }
    o.close();

    const std::string full_got = full_ctx.hex_final();
    if (full_got != full_sha) {
        std::error_code rec;
        fs::remove(tmp, rec);
        throw std::runtime_error("resolve_role: reassembled file sha " +
                                 full_got + " does not match manifest " +
                                 full_sha);
    }
    fs::rename(tmp, out, ec);
    if (ec) {
        throw std::runtime_error("resolve_role: rename failed: " + ec.message());
    }
    std::fprintf(stderr,
        "data: role \"%s\" reassembled to %s (%zu chunks, sha %s)\n",
        role.c_str(), out.c_str(), chunks.size(), full_sha.c_str());
    return out;
}

namespace {

// Case-insensitive substring test used for the "mmproj" name filter.
bool contains_ci(std::string_view h, std::string_view n) {
    if (n.empty() || n.size() > h.size()) return false;
    auto low = [](char c) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    };
    for (std::size_t i = 0; i + n.size() <= h.size(); ++i) {
        bool ok = true;
        for (std::size_t j = 0; j < n.size(); ++j) {
            if (low(h[i + j]) != low(n[j])) { ok = false; break; }
        }
        if (ok) return true;
    }
    return false;
}

// True iff every chunk .bin listed in `entry` exists under `data_dir`.
// Cheap on-disk existence check; no reassembly, no sha verification.
bool manifest_chunks_present(const fs::path & data_dir, const json & entry) {
    if (!entry.is_object()) return false;
    if (!entry.contains("chunks")) return false;
    const auto & chunks = entry["chunks"];
    if (!chunks.is_array() || chunks.empty()) return false;
    for (const auto & c : chunks) {
        if (!c.is_string()) return false;
        std::error_code ec;
        if (!fs::exists(data_dir / (c.get<std::string>() + ".bin"), ec)) {
            return false;
        }
    }
    return true;
}

// Scan resource_root/<role>/ for the largest top-level *.gguf that does
// NOT contain "mmproj" in its name. Returns empty path if the dir does
// not exist or has no candidates.
fs::path resource_llm_gguf(const fs::path & resource_root, const std::string & role) {
    fs::path dir = resource_root / role;
    std::error_code ec;
    if (!fs::is_directory(dir, ec)) return {};
    fs::path best;
    std::uintmax_t best_size = 0;
    for (auto it = fs::directory_iterator(dir, ec);
         !ec && it != fs::directory_iterator(); ++it) {
        if (!it->is_regular_file(ec)) continue;
        const std::string name = it->path().filename().string();
        if (name.size() < 5 ||
            name.compare(name.size() - 5, 5, ".gguf") != 0) continue;
        if (contains_ci(name, "mmproj")) continue;
        auto sz = it->file_size(ec);
        if (ec) continue;
        if (sz > best_size) { best_size = sz; best = it->path(); }
    }
    return best;
}

// Same, but pick the largest *.gguf whose name DOES contain "mmproj".
fs::path resource_mmproj_gguf(const fs::path & resource_root, const std::string & role) {
    fs::path dir = resource_root / role;
    std::error_code ec;
    if (!fs::is_directory(dir, ec)) return {};
    fs::path best;
    std::uintmax_t best_size = 0;
    for (auto it = fs::directory_iterator(dir, ec);
         !ec && it != fs::directory_iterator(); ++it) {
        if (!it->is_regular_file(ec)) continue;
        const std::string name = it->path().filename().string();
        if (name.size() < 5 ||
            name.compare(name.size() - 5, 5, ".gguf") != 0) continue;
        if (!contains_ci(name, "mmproj")) continue;
        auto sz = it->file_size(ec);
        if (ec) continue;
        if (sz > best_size) { best_size = sz; best = it->path(); }
    }
    return best;
}

// Scan resource_root/<role>/ for a <base>.gguf.part-*.bin pattern.
// Returns the derived <base>.gguf path (which may not yet exist) so the
// caller can hand it to model_chunks::ensure() for reassembly. If two
// bases coexist (e.g. an mmproj plus an LLM), prefer the one WITHOUT
// "mmproj" in its name when `want_mmproj` is false, and vice versa.
fs::path resource_reassembly_target(const fs::path   & resource_root,
                                    const std::string & role,
                                    bool               want_mmproj) {
    fs::path dir = resource_root / role;
    std::error_code ec;
    if (!fs::is_directory(dir, ec)) return {};
    fs::path best;
    for (auto it = fs::directory_iterator(dir, ec);
         !ec && it != fs::directory_iterator(); ++it) {
        if (!it->is_regular_file(ec)) continue;
        const std::string name = it->path().filename().string();
        const auto pos = name.find(".gguf.part-");
        if (pos == std::string::npos) continue;
        const std::string base = name.substr(0, pos + 5);   // "<name>.gguf"
        const bool has_mmproj = contains_ci(base, "mmproj");
        if (want_mmproj != has_mmproj) continue;
        best = dir / base;
        break;
    }
    return best;
}

}  // namespace (anon)

fs::path role_path(const std::string & role,
                   const fs::path    & data_dir,
                   const fs::path    & resource_root) {
    // 1. Manifest / sha-addressed chunks.
    {
        const fs::path mpath = data_dir / "manifest.json";
        std::ifstream mf(mpath);
        if (mf) {
            json manifest;
            try { mf >> manifest; } catch (...) { manifest = json::object(); }
            if (manifest.contains(role) &&
                manifest_chunks_present(data_dir, manifest[role])) {
                return resolve_role(role, data_dir);
            }
        }
    }
    // 2. Legacy resources/models/<role>/*.gguf top-level (largest, non-mmproj).
    if (auto p = resource_llm_gguf(resource_root, role); !p.empty()) {
        return p;
    }
    // 3. Legacy chunk pieces (<base>.gguf.part-*.bin) that need reassembly.
    if (auto cand = resource_reassembly_target(resource_root, role, false);
        !cand.empty()) {
        if (model_chunks::ensure(cand)) return cand;
    }
    throw std::runtime_error(
        "data::role_path: role \"" + role +
        "\" has no on-disk model (looked in " + (data_dir / "manifest.json").string() +
        " and " + (resource_root / role).string() + ")");
}

fs::path role_mmproj_path(const std::string & role,
                          const fs::path    & /*data_dir*/,
                          const fs::path    & resource_root) {
    if (auto p = resource_mmproj_gguf(resource_root, role); !p.empty()) {
        return p;
    }
    if (auto cand = resource_reassembly_target(resource_root, role, true);
        !cand.empty()) {
        if (model_chunks::ensure(cand)) return cand;
    }
    return {};
}

bool role_available(const std::string & role,
                    const fs::path    & data_dir,
                    const fs::path    & resource_root) {
    // Cheap: no reassembly, no sha verification. Manifest presence is
    // enough to promise availability -- resolve_role will rebuild the
    // cache on first real load.
    {
        const fs::path mpath = data_dir / "manifest.json";
        std::ifstream mf(mpath);
        if (mf) {
            json manifest;
            try { mf >> manifest; } catch (...) { manifest = json::object(); }
            if (manifest.contains(role) &&
                manifest_chunks_present(data_dir, manifest[role])) {
                return true;
            }
        }
    }
    if (!resource_llm_gguf(resource_root, role).empty())      return true;
    if (!resource_reassembly_target(resource_root, role, false).empty())
        return true;
    return false;
}

std::vector<RoleInfo> list_available_roles(const fs::path & data_dir,
                                           const fs::path & resource_root) {
    std::vector<RoleInfo> out;
    std::unordered_map<std::string, std::size_t> seen;

    auto short_name_from = [](const json & entry) -> std::string {
        if (entry.is_object() && entry.contains("short_name") &&
            entry["short_name"].is_string()) return entry["short_name"].get<std::string>();
        return {};
    };
    auto human_name_from = [](const json & entry) -> std::string {
        if (entry.is_object() && entry.contains("human_name") &&
            entry["human_name"].is_string()) return entry["human_name"].get<std::string>();
        return {};
    };

    // 1. Manifest-driven roles that have chunks-on-disk.
    json manifest = json::object();
    {
        std::ifstream f(data_dir / "manifest.json");
        if (f) { try { f >> manifest; } catch (...) { manifest = json::object(); } }
    }
    if (manifest.is_object()) {
        for (auto it = manifest.begin(); it != manifest.end(); ++it) {
            if (!manifest_chunks_present(data_dir, it.value())) continue;
            RoleInfo ri;
            ri.role       = it.key();
            ri.human_name = human_name_from(it.value());
            ri.short_name = short_name_from(it.value());
            if (it.value().is_object() && it.value().contains("size_bytes") &&
                it.value()["size_bytes"].is_number_unsigned()) {
                ri.size_bytes = it.value()["size_bytes"].get<std::uintmax_t>();
            }
            ri.source     = "manifest";
            ri.has_mmproj = false;
            seen[ri.role] = out.size();
            out.push_back(std::move(ri));
        }
    }

    // 2. resources/models/<role>/ directories with any usable *.gguf.
    // Cross-reference sources.json for short_name / human_name if the
    // role has no manifest entry to draw those from.
    json sources = json::object();
    {
        std::ifstream f(data_dir / "sources.json");
        if (f) { try { f >> sources; } catch (...) { sources = json::object(); } }
    }

    std::error_code ec;
    if (fs::is_directory(resource_root, ec)) {
        for (auto it = fs::directory_iterator(resource_root, ec);
             !ec && it != fs::directory_iterator(); ++it) {
            if (!it->is_directory(ec)) continue;
            const std::string role = it->path().filename().string();
            fs::path llm = resource_llm_gguf(resource_root, role);
            if (llm.empty()) {
                // Try reassembly-from-parts target existence (chunks only).
                if (resource_reassembly_target(resource_root, role, false).empty()) {
                    continue;
                }
            }
            fs::path mmproj = resource_mmproj_gguf(resource_root, role);
            if (mmproj.empty()) {
                mmproj = resource_reassembly_target(resource_root, role, true);
            }
            auto found = seen.find(role);
            if (found != seen.end()) {
                // Manifest already listed it; annotate mmproj if we found one.
                out[found->second].has_mmproj = !mmproj.empty();
                continue;
            }
            RoleInfo ri;
            ri.role       = role;
            std::uintmax_t sz = 0;
            if (!llm.empty()) {
                sz = fs::file_size(llm, ec); if (ec) sz = 0;
                ri.human_name = llm.filename().string();
            }
            if (sources.is_object() && sources.contains(role)) {
                ri.short_name = short_name_from(sources[role]);
                if (ri.human_name.empty()) {
                    ri.human_name = human_name_from(sources[role]);
                }
                if (sz == 0 && sources[role].is_object() &&
                    sources[role].contains("expected_size_bytes") &&
                    sources[role]["expected_size_bytes"].is_number_unsigned()) {
                    sz = sources[role]["expected_size_bytes"].get<std::uintmax_t>();
                }
            }
            ri.size_bytes = sz;
            ri.source     = "resource";
            ri.has_mmproj = !mmproj.empty();
            seen[role]    = out.size();
            out.push_back(std::move(ri));
        }
    }

    // Stable alphabetic order so the UI dropdown reads the same across
    // process restarts.
    std::sort(out.begin(), out.end(),
              [](const RoleInfo & a, const RoleInfo & b) {
                  return a.role < b.role;
              });
    return out;
}

int cli_chunk(int argc, char ** argv) {
    // Expected: argv[0]=ac9  argv[1]=chunk  argv[2]=role  argv[3]=name
    //           argv[4]=input  [argv[5]=data_dir]
    if (argc < 5) {
        std::fprintf(stderr,
            "usage: %s chunk <role> <human_name> <input_path> [data_dir]\n"
            "  role         short identifier (coder, planner-4b, ...)\n"
            "  human_name   original filename (for the manifest)\n"
            "  input_path   file to chunk\n"
            "  data_dir     defaults to ./data\n",
            argv[0]);
        return 2;
    }
    const std::string role      = argv[2];
    const std::string human     = argv[3];
    const fs::path    input     = argv[4];
    const fs::path    data_dir  = (argc >= 6) ? fs::path(argv[5])
                                              : fs::path("data");
    try {
        auto res = chunk_asset(input, data_dir);
        set_role_in_manifest(data_dir, role, human, res);
        std::fprintf(stderr,
            "data: role \"%s\" -> %zu chunks, full sha256 %s\n",
            role.c_str(), res.chunk_shas.size(),
            res.full_sha256.c_str());
        return 0;
    } catch (const std::exception & ex) {
        std::fprintf(stderr, "data: %s\n", ex.what());
        return 1;
    }
}

}  // namespace data
