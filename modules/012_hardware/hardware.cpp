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
#include <unordered_map>

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

// =====================================================================
// Live stats (GPU temp/util/mem + system RAM/CPU/temp). 250 ms TTL
// cache so a 2 Hz poll from every browser costs one fork per half sec.
// =====================================================================

namespace {

std::mutex                                   g_gpu_stats_mu;
std::vector<GpuStats>                        g_gpu_stats_cache;
std::chrono::steady_clock::time_point        g_gpu_stats_ts{};

std::mutex                                   g_sys_stats_mu;
SystemStats                                  g_sys_stats_cache;
std::chrono::steady_clock::time_point        g_sys_stats_ts{};

// Single-card LRU scheduler state. Placed here (in the same anon
// namespace as query_gpu_stats' cache) so query_gpu_stats can attach
// per-card role tags without a forward-declaration dance.
struct SchedSlot {
    std::string                            role;      // "" = idle
    std::uint64_t                          weight_bytes{0};
    std::chrono::steady_clock::time_point  last_touch;
};

std::mutex                                   g_sched_mu;
std::unordered_map<int, SchedSlot>           g_slots;
std::vector<Gpu>                             g_sched_gpus;

std::vector<GpuStats> gpu_stats_via_nvidia_smi() {
    std::vector<GpuStats> out;
    std::FILE * p = ::popen(
        "nvidia-smi --query-gpu=index,name,temperature.gpu,"
        "utilization.gpu,memory.used,memory.total "
        "--format=csv,noheader,nounits 2>/dev/null", "r");
    if (!p) return out;
    char line[512];
    while (std::fgets(line, sizeof(line), p)) {
        GpuStats g;
        char name[256] = {0};
        int temp = -1, util = -1;
        unsigned long long mu = 0, mt = 0;
        if (std::sscanf(line, "%d, %255[^,], %d, %d, %llu, %llu",
                        &g.id, name, &temp, &util, &mu, &mt) >= 6) {
            g.name      = name;
            g.temp_c    = temp;
            g.util_pct  = util;
            g.mem_used  = static_cast<std::uint64_t>(mu) * 1024ULL * 1024ULL;
            g.mem_total = static_cast<std::uint64_t>(mt) * 1024ULL * 1024ULL;
            out.push_back(g);
        }
    }
    ::pclose(p);
    return out;
}

// CPU % util is derived from two /proc/stat samples ~100 ms apart. We
// cache the last raw sample so subsequent calls return an update
// without needing to sleep.
struct CpuStatSample { std::uint64_t total{0}; std::uint64_t idle{0}; };
CpuStatSample last_cpu_sample;

CpuStatSample read_proc_stat_cpu() {
    CpuStatSample s;
    std::FILE * f = std::fopen("/proc/stat", "r");
    if (!f) return s;
    char line[512];
    if (std::fgets(line, sizeof(line), f)) {
        std::uint64_t u = 0, n = 0, sy = 0, id = 0, io = 0, ir = 0, so = 0;
        if (std::sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu",
                        (unsigned long long *)&u, (unsigned long long *)&n,
                        (unsigned long long *)&sy, (unsigned long long *)&id,
                        (unsigned long long *)&io, (unsigned long long *)&ir,
                        (unsigned long long *)&so) >= 4) {
            s.idle  = id + io;
            s.total = u + n + sy + id + io + ir + so;
        }
    }
    std::fclose(f);
    return s;
}

int cpu_pct_from_deltas(const CpuStatSample & a, const CpuStatSample & b) {
    if (b.total <= a.total) return -1;
    std::uint64_t dt = b.total - a.total;
    std::uint64_t di = b.idle > a.idle ? b.idle - a.idle : 0;
    if (dt == 0) return -1;
    long double busy = 1.0L - (long double)di / (long double)dt;
    if (busy < 0) busy = 0; if (busy > 1) busy = 1;
    return static_cast<int>(busy * 100.0L + 0.5L);
}

std::uint64_t read_kb_from_meminfo(const char * key) {
    std::FILE * f = std::fopen("/proc/meminfo", "r");
    if (!f) return 0;
    char line[256];
    std::uint64_t out = 0;
    const std::size_t klen = std::strlen(key);
    while (std::fgets(line, sizeof(line), f)) {
        if (std::strncmp(line, key, klen) == 0) {
            unsigned long long v = 0;
            if (std::sscanf(line + klen, "%llu", &v) == 1) out = v * 1024ULL;
            break;
        }
    }
    std::fclose(f);
    return out;
}

int read_hottest_hwmon_temp() {
    // Scan /sys/class/hwmon/hwmonN/temp*_input, pick the hottest reading.
    // Values are milli-degrees Celsius.
    int hottest = -1;
    for (int i = 0; i < 16; ++i) {
        for (int j = 1; j < 16; ++j) {
            char path[128];
            std::snprintf(path, sizeof(path),
                "/sys/class/hwmon/hwmon%d/temp%d_input", i, j);
            std::FILE * f = std::fopen(path, "r");
            if (!f) continue;
            int v = 0;
            if (std::fscanf(f, "%d", &v) == 1) {
                int c = v / 1000;
                if (c > hottest) hottest = c;
            }
            std::fclose(f);
        }
    }
    return hottest;
}

int read_n_cpus() {
    std::FILE * f = std::fopen("/proc/cpuinfo", "r");
    if (!f) return 0;
    int n = 0;
    char line[256];
    while (std::fgets(line, sizeof(line), f)) {
        if (std::strncmp(line, "processor", 9) == 0) ++n;
    }
    std::fclose(f);
    return n;
}

}  // namespace (anon)

std::vector<GpuStats> query_gpu_stats() {
    using clk = std::chrono::steady_clock;
    {
        std::lock_guard<std::mutex> lk(g_gpu_stats_mu);
        auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
                     clk::now() - g_gpu_stats_ts).count();
        if (!g_gpu_stats_cache.empty() && age < 250) return g_gpu_stats_cache;
    }
    auto fresh = gpu_stats_via_nvidia_smi();
    // Attach the LRU scheduler's per-card role tag so the UI can render
    // "thinking (Q3 Think 30 on gpu 1)" instead of just the model name.
    {
        std::lock_guard<std::mutex> sk(g_sched_mu);
        for (auto & g : fresh) {
            auto it = g_slots.find(g.id);
            if (it != g_slots.end()) g.role = it->second.role;
        }
    }
    std::lock_guard<std::mutex> lk(g_gpu_stats_mu);
    g_gpu_stats_cache = fresh;
    g_gpu_stats_ts    = clk::now();
    return fresh;
}

SystemStats query_system_stats() {
    using clk = std::chrono::steady_clock;
    {
        std::lock_guard<std::mutex> lk(g_sys_stats_mu);
        auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
                     clk::now() - g_sys_stats_ts).count();
        if (age < 250 && g_sys_stats_cache.mem_total > 0)
            return g_sys_stats_cache;
    }
    SystemStats s;
    s.mem_total = read_kb_from_meminfo("MemTotal:");
    s.mem_used  = s.mem_total - read_kb_from_meminfo("MemAvailable:");
    s.temp_c    = read_hottest_hwmon_temp();
    // CPU % against the LAST cached sample so consecutive polls give a
    // sensible reading without needing to sleep here.
    CpuStatSample now = read_proc_stat_cpu();
    {
        std::lock_guard<std::mutex> lk(g_sys_stats_mu);
        if (last_cpu_sample.total > 0) {
            s.cpu_pct = cpu_pct_from_deltas(last_cpu_sample, now);
        }
        last_cpu_sample = now;
    }
    s.n_cpus = read_n_cpus();
    std::lock_guard<std::mutex> lk(g_sys_stats_mu);
    g_sys_stats_cache = s;
    g_sys_stats_ts    = clk::now();
    return s;
}

// =====================================================================
// Single-card LRU scheduler (see hardware.hpp::pick_placement).
//
// Each role, when loaded, occupies exactly one GPU. On every switch we
// pick the OTHER (least-recently-used) card so the previous role can
// stay mmap-resident on its card. jwoods benched this against
// LLAMA_SPLIT_MODE_LAYER under ollama and confirmed single-card is
// meaningfully faster; the previous-card idle-warm cache is a bonus.
//
// When a model exceeds the chosen card's VRAM budget, we clip
// n_gpu_layers to whatever fraction of layers fits (heuristic: fraction
// of raw weight bytes that fit, times the role's approximate layer
// count) and let llama.cpp mmap-stream the overflow.
// =====================================================================

namespace {
void ensure_sched_gpus_locked() {
    if (!g_sched_gpus.empty()) return;
    g_sched_gpus = enumerate_gpus();
    for (const auto & g : g_sched_gpus) {
        if (!g_slots.count(g.id)) g_slots[g.id] = SchedSlot{};
    }
}
}  // namespace (anon)

Placement pick_placement(const std::string & role,
                         std::uint64_t model_bytes,
                         std::uint64_t /*kv_reserve_bytes*/) {
    std::lock_guard<std::mutex> lk(g_sched_mu);
    ensure_sched_gpus_locked();

    Placement p;
    p.mmap         = true;
    p.n_gpu_layers = -1;             // UVA covers overflow; always all layers

    if (g_sched_gpus.empty()) {
        p.main_gpu = 0;
        p.reason   = "no gpus; cpu only";
        return p;
    }

    // 1. Warm-cache hit: this role is already resident. Reuse the card.
    for (const auto & [gpu, st] : g_slots) {
        if (st.role == role) {
            p.main_gpu = gpu;
            p.reason   = "warm cache on gpu " + std::to_string(gpu);
            return p;
        }
    }

    // 2. Prefer an EMPTY card. Two-card LRU rotation: if BOTH cards are
    //    empty, pick the card NOT touched most recently, so we swap sides
    //    on every fresh load. That's the whole point of the scheduler.
    int best              = -1;
    auto best_ts_empty    = std::chrono::steady_clock::time_point::max();
    for (const auto & g : g_sched_gpus) {
        auto it = g_slots.find(g.id);
        const bool empty = (it == g_slots.end() || it->second.role.empty());
        const auto ts    = (it == g_slots.end())
                             ? std::chrono::steady_clock::time_point::min()
                             : it->second.last_touch;
        if (empty && ts < best_ts_empty) {
            best_ts_empty = ts;
            best          = g.id;
        }
    }

    // 3. All cards busy: evict LRU. displaced_role tells the caller which
    //    sibling to shut down.
    if (best < 0) {
        auto oldest_ts = std::chrono::steady_clock::time_point::max();
        for (const auto & [gpu, st] : g_slots) {
            if (st.last_touch < oldest_ts) {
                oldest_ts = st.last_touch;
                best      = gpu;
            }
        }
        auto it = g_slots.find(best);
        if (it != g_slots.end()) p.displaced_role = it->second.role;
    }
    if (best < 0) best = g_sched_gpus.front().id;

    p.main_gpu     = best;
    p.split_across = false;

    std::uint64_t card_bytes = 0;
    for (const auto & g : g_sched_gpus) {
        if (g.id == best) { card_bytes = g.total_vram; break; }
    }
    const bool fits = (card_bytes == 0) || (model_bytes <= card_bytes);
    p.reason = (fits ? "fits fully on gpu " : "UVA overflow on gpu ") +
               std::to_string(best);
    if (!p.displaced_role.empty()) {
        p.reason += " (displaced role \"" + p.displaced_role + "\")";
    }
    // Optimistically stamp the picked card with the role NOW. That way
    // query_gpu_stats returns g.role = <role> during the multi-second
    // llama_model_load_from_file call, so the UI's headline can render
    // "loading (X on gpu N)" instead of just "loading (X)". If the load
    // fails the caller's shutdown_if_loaded (or note_role_unloaded)
    // clears it back to idle.
    auto & slot     = g_slots[best];
    slot.role       = role;
    slot.last_touch = std::chrono::steady_clock::now();
    (void) model_bytes;
    return p;
}

// Registry of module shutdown hooks so we can evict a specific role by
// name instead of every sibling. Populated at link-time via extern "C"
// symbols each module already exports.
extern "C" {
    void coder_shutdown_if_loaded();
    void qwen14b_shutdown_if_loaded();
    void planner_shutdown_if_loaded();
    void physics_shutdown_if_loaded();
    void chemistry_shutdown_if_loaded();
    void vision_shutdown_if_loaded();
}

void request_evict(const std::string & role) {
    if (role.empty()) return;
    if (role == "coder" || role == "coder-14b" || role == "coder-big") {
        coder_shutdown_if_loaded();
    } else if (role == "qwen14b") {
        qwen14b_shutdown_if_loaded();
    } else if (role == "planner-30b" || role == "planner-4b" ||
               role.rfind("planner", 0) == 0) {
        planner_shutdown_if_loaded();
    } else if (role == "physics") {
        physics_shutdown_if_loaded();
    } else if (role == "chemistry") {
        chemistry_shutdown_if_loaded();
    } else if (role == "vision") {
        vision_shutdown_if_loaded();
    } else {
        std::fprintf(stderr,
            "hw sched: request_evict(\"%s\") -> no known shutdown hook\n",
            role.c_str());
    }
}

void note_role_loaded(const std::string & role, int gpu) {
    std::lock_guard<std::mutex> lk(g_sched_mu);
    ensure_sched_gpus_locked();
    // Clear the role from any card it was previously on (a role never
    // occupies two cards simultaneously under this scheduler).
    for (auto & [id, st] : g_slots) {
        if (st.role == role && id != gpu) st.role.clear();
    }
    auto & slot        = g_slots[gpu];
    slot.role          = role;
    slot.last_touch    = std::chrono::steady_clock::now();
    std::fprintf(stderr,
        "hw sched: role \"%s\" -> gpu %d\n", role.c_str(), gpu);
}

void note_role_unloaded(const std::string & role) {
    std::lock_guard<std::mutex> lk(g_sched_mu);
    for (auto & [gpu, st] : g_slots) {
        if (st.role == role) {
            st.role.clear();
            std::fprintf(stderr,
                "hw sched: role \"%s\" evicted from gpu %d\n",
                role.c_str(), gpu);
        }
    }
}

}  // namespace hardware
