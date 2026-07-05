// SPDX-License-Identifier: GPL-3.0-or-later
#include "hardware.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <map>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>

#if __has_include(<cuda_runtime.h>)
#  include <cuda_runtime.h>
#  define HW_HAVE_CUDA 1
#else
#  define HW_HAVE_CUDA 0
#endif

namespace hardware {
namespace fs = std::filesystem;
using json    = nlohmann::json;

namespace {

// Leave some VRAM headroom for KV cache growth, activations, etc.
constexpr std::size_t kSafetyBytes   = 512ULL * 1024 * 1024;   // 512 MiB
// Rough model overhead beyond raw weight bytes (compute buffers etc.).
constexpr std::size_t kOverheadBytes = 256ULL * 1024 * 1024;   // 256 MiB
// KV cache estimate: 8 KB per layer per k-tokens of context.
// Good enough for planning, real usage depends on n_head_kv * head_dim.
constexpr std::size_t kKVBytesPerLayerPerKTokens = 8ULL * 1024;

std::mutex g_plan_mtx;
json       g_plan;

// Class hints keyed by role name. Anything unlisted -> "ON_DEMAND".
// - HOT: kept resident once loaded (coder, qwen14b, cleanup).
// - SWAP: unloaded to make room for other SWAP peers on the same card
//         (physics, chemistry, planner-30b).
// - COLOCATE_PAIR: must share a GPU with a partner (vision + mmproj).
// - ON_DEMAND: load when needed, no explicit reservation.
struct RoleHint {
    std::string klass;
    std::string co_group;
    int         est_ctx{4096};
};
const std::map<std::string, RoleHint> & role_hints() {
    static const std::map<std::string, RoleHint> h = {
        { "cleanup",       { "HOT",           "",         4096 } },
        { "qwen14b",       { "HOT",           "",         4096 } },
        { "coder",         { "HOT",           "",         8192 } },
        { "physics",       { "SWAP",          "",         8192 } },
        { "chemistry",     { "SWAP",          "",         4096 } },
        { "planner-4b",    { "ON_DEMAND",     "",         8192 } },
        { "planner-30b",   { "SWAP",          "",         8192 } },
        { "vision",        { "COLOCATE_PAIR", "vision",   8192 } },
        { "vision-mmproj", { "COLOCATE_PAIR", "vision",   8192 } },
    };
    return h;
}
RoleHint hint_for(const std::string & role) {
    const auto & m = role_hints();
    auto it = m.find(role);
    if (it != m.end()) return it->second;
    return RoleHint{"ON_DEMAND", "", 4096};
}

std::string iso_now() {
    char b[32]; std::time_t t = std::time(nullptr); std::tm g{};
    gmtime_r(&t, &g);
    std::strftime(b, sizeof(b), "%Y-%m-%dT%H:%M:%SZ", &g);
    return b;
}

// ---- GPU enumeration -----------------------------------------------------

std::vector<Gpu> enumerate_via_cuda() {
    std::vector<Gpu> out;
#if HW_HAVE_CUDA
    int n = 0;
    if (cudaGetDeviceCount(&n) != cudaSuccess) return out;
    for (int i = 0; i < n; ++i) {
        cudaDeviceProp p{};
        if (cudaGetDeviceProperties(&p, i) != cudaSuccess) continue;
        Gpu g;
        g.id         = i;
        g.name       = p.name;
        g.total_vram = p.totalGlobalMem;
        g.cc_major   = p.major;
        g.cc_minor   = p.minor;
        std::size_t free_b = 0, total_b = 0;
        if (cudaSetDevice(i) == cudaSuccess &&
            cudaMemGetInfo(&free_b, &total_b) == cudaSuccess) {
            g.free_vram = free_b;
        } else {
            g.free_vram = p.totalGlobalMem;
        }
        out.push_back(std::move(g));
    }
    if (!out.empty()) cudaSetDevice(0);
#endif
    return out;
}

std::vector<Gpu> enumerate_via_nvidia_smi() {
    std::vector<Gpu> out;
    std::FILE * pipe = ::popen(
        "nvidia-smi --query-gpu=index,name,memory.total,memory.free,"
        "compute_cap --format=csv,noheader,nounits 2>/dev/null", "r");
    if (!pipe) return out;
    char line[512];
    while (std::fgets(line, sizeof(line), pipe)) {
        Gpu g;
        char name[128] = {0};
        int idx = 0;
        unsigned long tot_mib = 0, free_mib = 0;
        float cc = 0;
        if (std::sscanf(line, "%d, %127[^,], %lu, %lu, %f",
                        &idx, name, &tot_mib, &free_mib, &cc) >= 4) {
            g.id         = idx;
            g.name       = name;
            g.total_vram = static_cast<std::size_t>(tot_mib) * 1024ULL * 1024ULL;
            g.free_vram  = static_cast<std::size_t>(free_mib) * 1024ULL * 1024ULL;
            g.cc_major   = static_cast<int>(cc);
            g.cc_minor   = static_cast<int>((cc - g.cc_major) * 10 + 0.5f);
            out.push_back(std::move(g));
        }
    }
    ::pclose(pipe);
    return out;
}

// ---- Planning helpers ----------------------------------------------------

struct Role {
    std::string name;
    std::size_t size_bytes{0};
    RoleHint    hint;
    // Estimated VRAM to hold the model in full: on-disk + KV + overhead.
    std::size_t est_full_vram() const {
        std::size_t kv_ktokens = std::max(1, hint.est_ctx / 1024);
        // Rough proxy for layer count: 30 layers per ~10 GB of weights.
        std::size_t layers = std::max<std::size_t>(1, size_bytes / (350ULL * 1024 * 1024));
        std::size_t kv = layers * kv_ktokens * kKVBytesPerLayerPerKTokens;
        return size_bytes + kv + kOverheadBytes;
    }
};

int pick_gpu_bestfit(const std::vector<Gpu>          & gpus,
                     const std::vector<std::size_t>  & reserved,
                     std::size_t                       need) {
    int best = -1;
    std::size_t best_slack = SIZE_MAX;
    for (std::size_t i = 0; i < gpus.size(); ++i) {
        std::size_t avail = gpus[i].free_vram > reserved[i]
                          ? gpus[i].free_vram - reserved[i] : 0;
        if (avail < kSafetyBytes + need) continue;
        std::size_t slack = avail - need;
        if (slack < best_slack) { best = static_cast<int>(i); best_slack = slack; }
    }
    return best;
}

int pick_gpu_largest(const std::vector<Gpu>          & gpus,
                     const std::vector<std::size_t>  & reserved) {
    int best = -1;
    long long best_free = -1;
    for (std::size_t i = 0; i < gpus.size(); ++i) {
        long long f = static_cast<long long>(gpus[i].free_vram) -
                      static_cast<long long>(reserved[i]);
        if (f > best_free) { best_free = f; best = static_cast<int>(i); }
    }
    return best;
}

}  // namespace

std::vector<Gpu> enumerate_gpus() {
    auto v = enumerate_via_cuda();
    if (v.empty()) v = enumerate_via_nvidia_smi();
    return v;
}

json plan_roles(const std::vector<Gpu> & gpus_in,
                const fs::path         & manifest_path) {
    json manifest = json::object();
    {
        std::ifstream f(manifest_path);
        if (f) { try { f >> manifest; } catch (...) { manifest = json::object(); } }
    }

    std::vector<Role> roles;
    for (auto it = manifest.begin(); it != manifest.end(); ++it) {
        const std::string name = it.key();
        if (name.empty() || name[0] == '_') continue;
        Role r;
        r.name       = name;
        r.size_bytes = it.value().value("size_bytes", 0ULL);
        r.hint       = hint_for(name);
        roles.push_back(std::move(r));
    }

    std::vector<Gpu>          gpus     = gpus_in;
    std::vector<std::size_t>  reserved(gpus.size(), 0);

    json out;
    out["generated_at"] = iso_now();
    out["gpus"]         = json::array();
    for (const auto & g : gpus) {
        out["gpus"].push_back({
            {"id",         g.id},
            {"name",       g.name},
            {"total_vram", g.total_vram},
            {"free_vram",  g.free_vram},
            {"cc_major",   g.cc_major},
            {"cc_minor",   g.cc_minor}});
    }
    out["roles"]       = json::object();
    out["swap_groups"] = json::object();

    // If no GPUs, everything is CPU-only.
    if (gpus.empty()) {
        for (const auto & r : roles) {
            out["roles"][r.name] = {
                {"gpu",          -1},
                {"n_gpu_layers", 0},
                {"mode",         "CPU"},
                {"mmap",         true},
                {"class",        r.hint.klass}};
        }
        return out;
    }

    auto place_full = [&](const Role & r, int gi) {
        out["roles"][r.name] = {
            {"gpu",          gi},
            {"n_gpu_layers", 999},
            {"mode",         "FULL"},
            {"mmap",         true},
            {"class",        r.hint.klass}};
        reserved[gi] += r.est_full_vram();
    };
    auto place_partial = [&](const Role & r, int gi, std::size_t avail) {
        std::size_t bpl = std::max<std::size_t>(
            1, r.size_bytes /
               std::max<std::size_t>(1, r.size_bytes / (350ULL * 1024 * 1024)));
        int layers_fit = static_cast<int>((avail > kSafetyBytes ?
                                           avail - kSafetyBytes : 0) / bpl);
        out["roles"][r.name] = {
            {"gpu",          gi},
            {"n_gpu_layers", std::max(0, layers_fit)},
            {"mode",         "PARTIAL"},
            {"mmap",         true},
            {"class",        r.hint.klass}};
        reserved[gi] += static_cast<std::size_t>(std::max(0, layers_fit)) * bpl;
    };
    auto place_one = [&](const Role & r) {
        std::size_t need = r.est_full_vram();
        int gi = pick_gpu_bestfit(gpus, reserved, need);
        if (gi >= 0) { place_full(r, gi); return; }
        gi = pick_gpu_largest(gpus, reserved);
        if (gi < 0)  gi = 0;
        std::size_t avail = gpus[gi].free_vram > reserved[gi]
                          ? gpus[gi].free_vram - reserved[gi] : 0;
        place_partial(r, gi, avail);
    };

    // Order: HOT first (fixed residency), then COLOCATE (paired), then
    // ON_DEMAND, then SWAP (share a slot per GPU).
    std::vector<const Role *> hot, on_demand, swap;
    std::map<std::string, std::vector<const Role *>> colo;
    for (const auto & r : roles) {
        if      (r.hint.klass == "HOT")            hot.push_back(&r);
        else if (r.hint.klass == "SWAP")           swap.push_back(&r);
        else if (r.hint.klass == "COLOCATE_PAIR") {
            colo[r.hint.co_group.empty() ? r.name : r.hint.co_group]
                .push_back(&r);
        } else {
            on_demand.push_back(&r);
        }
    }
    auto desc = [](std::vector<const Role *> & v) {
        std::sort(v.begin(), v.end(), [](const Role * a, const Role * b) {
            return a->est_full_vram() > b->est_full_vram();
        });
    };
    desc(hot); desc(on_demand); desc(swap);

    for (const Role * r : hot) place_one(*r);
    for (const auto & kv : colo) {
        std::size_t need = 0;
        for (const Role * r : kv.second) need += r->est_full_vram();
        int gi = pick_gpu_bestfit(gpus, reserved, need);
        if (gi < 0) gi = pick_gpu_largest(gpus, reserved);
        if (gi < 0) gi = 0;
        for (const Role * r : kv.second) {
            out["roles"][r->name] = {
                {"gpu",          gi},
                {"n_gpu_layers", 999},
                {"mode",         "FULL"},
                {"mmap",         true},
                {"co_group",     kv.first},
                {"class",        r->hint.klass}};
            reserved[gi] += r->est_full_vram();
        }
    }
    for (const Role * r : on_demand) place_one(*r);

    // SWAP: one shared slot per GPU, size = max(costs).
    std::map<int, std::size_t>              slot_peak;
    std::map<int, std::vector<std::string>> slot_members;
    for (const Role * r : swap) {
        int gi = pick_gpu_largest(gpus, reserved);
        if (gi < 0) gi = 0;
        std::size_t need   = r->est_full_vram();
        std::size_t was    = slot_peak[gi];
        std::size_t nowmax = std::max(was, need);
        std::size_t delta  = nowmax - was;
        std::size_t avail  = gpus[gi].free_vram > reserved[gi]
                           ? gpus[gi].free_vram - reserved[gi] : 0;
        if (delta > (avail > kSafetyBytes ? avail - kSafetyBytes : 0)) {
            std::size_t bpl = std::max<std::size_t>(
                1, r->size_bytes / std::max<std::size_t>(1,
                    r->size_bytes / (350ULL * 1024 * 1024)));
            int layers_fit = static_cast<int>((avail > kSafetyBytes ?
                                               avail - kSafetyBytes : 0) / bpl);
            out["roles"][r->name] = {
                {"gpu",          gi},
                {"n_gpu_layers", std::max(0, layers_fit)},
                {"mode",         "SWAP_PARTIAL"},
                {"mmap",         true},
                {"class",        r->hint.klass},
                {"swap_slot",    gi}};
        } else {
            out["roles"][r->name] = {
                {"gpu",          gi},
                {"n_gpu_layers", 999},
                {"mode",         "SWAP"},
                {"mmap",         true},
                {"class",        r->hint.klass},
                {"swap_slot",    gi}};
            reserved[gi] += delta;
            slot_peak[gi] = nowmax;
            slot_members[gi].push_back(r->name);
        }
    }
    for (auto & kv : slot_members) {
        out["swap_groups"][std::to_string(kv.first)] = {
            {"peak_bytes",     slot_peak[kv.first]},
            {"members",        kv.second},
            {"policy",         "LRU"},
            {"keep_mmap_warm", true}};
    }
    for (std::size_t i = 0; i < gpus.size(); ++i) {
        out["gpus"][i]["reserved_bytes"] = reserved[i];
    }
    return out;
}

void save_plan(const json & j, const fs::path & to) {
    std::error_code ec;
    if (to.has_parent_path()) fs::create_directories(to.parent_path(), ec);
    fs::path tmp = to; tmp += ".tmp";
    { std::ofstream o(tmp, std::ios::binary | std::ios::trunc); o << j.dump(2) << "\n"; }
    fs::rename(tmp, to, ec);
}

json load_plan(const fs::path & from) {
    std::ifstream f(from);
    if (!f) return json::object();
    try { json j; f >> j; return j; } catch (...) { return json::object(); }
}

void init() {
    std::vector<Gpu> gpus;
    fs::path         manifest = "data/manifest.json";
    fs::path         plan_path;
    if (const char * home = std::getenv("HOME"); home && *home) {
        plan_path = fs::path(home) / ".ac9" / "gpu_plan.json";
    } else {
        plan_path = fs::path("/tmp") / ".ac9" / "gpu_plan.json";
    }
    try { gpus = enumerate_gpus(); }
    catch (const std::exception & e) {
        std::fprintf(stderr, "hardware: gpu enumeration failed: %s\n", e.what());
    }
    json plan;
    try { plan = plan_roles(gpus, manifest); }
    catch (const std::exception & e) {
        std::fprintf(stderr, "hardware: plan failed: %s\n", e.what());
        return;
    }
    save_plan(plan, plan_path);
    {
        std::lock_guard<std::mutex> lk(g_plan_mtx);
        g_plan = std::move(plan);
    }
    std::fprintf(stderr,
        "hardware: %zu GPU(s) detected; plan saved to %s\n",
        gpus.size(), plan_path.c_str());
}

json role_assignment(const std::string & role) {
    std::lock_guard<std::mutex> lk(g_plan_mtx);
    if (g_plan.is_null() || !g_plan.contains("roles")) {
        fs::path plan_path;
        if (const char * home = std::getenv("HOME"); home && *home) {
            plan_path = fs::path(home) / ".ac9" / "gpu_plan.json";
        } else {
            plan_path = fs::path("/tmp") / ".ac9" / "gpu_plan.json";
        }
        g_plan = load_plan(plan_path);
    }
    if (g_plan.contains("roles") && g_plan["roles"].contains(role))
        return g_plan["roles"][role];
    // Safe default: full offload on GPU 0, or CPU if none.
    return json{{"gpu", 0}, {"n_gpu_layers", 999},
                {"mode", "FULL"}, {"mmap", true}};
}

int cli_hw(int /*argc*/, char ** /*argv*/) {
    auto v = enumerate_gpus();
    if (v.empty()) { std::fprintf(stderr, "no CUDA GPUs detected\n"); return 0; }
    for (const auto & g : v) {
        std::fprintf(stdout,
            "gpu %d  %-32s  total=%6.2f GiB  free=%6.2f GiB  cc=%d.%d\n",
            g.id, g.name.c_str(),
            g.total_vram / 1073741824.0,
            g.free_vram  / 1073741824.0,
            g.cc_major, g.cc_minor);
    }
    return 0;
}

int cli_plan(int argc, char ** argv) {
    fs::path plan_path;
    if (const char * home = std::getenv("HOME"); home && *home) {
        plan_path = fs::path(home) / ".ac9" / "gpu_plan.json";
    } else {
        plan_path = fs::path("/tmp") / ".ac9" / "gpu_plan.json";
    }
    bool refresh = false;
    for (int i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "--refresh") == 0) refresh = true;
    }
    if (refresh) {
        auto gpus = enumerate_gpus();
        json p    = plan_roles(gpus, "data/manifest.json");
        save_plan(p, plan_path);
        std::printf("%s\n", p.dump(2).c_str());
    } else {
        std::printf("%s\n", load_plan(plan_path).dump(2).c_str());
    }
    return 0;
}

}  // namespace hardware
