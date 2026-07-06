// SPDX-License-Identifier: GPL-3.0-or-later
#include "data.hpp"

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
