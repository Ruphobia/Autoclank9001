// SPDX-License-Identifier: GPL-3.0-or-later
#include "spice_simulator.hpp"

#include <atomic>
#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace spice_simulator {
namespace {

// Mirrors the ngspice shared-library ABI (see include/ngspice/sharedspice.h).
// We only declare the pieces we call.
extern "C" {
    struct vector_info {
        char *  v_name;
        int     v_type;
        short   v_flags;
        double * v_realdata;
        void  * v_compdata; // ngcomplex_t*
        int     v_length;
    };
    typedef int (*SendChar_fn)     (char*, int, void*);
    typedef int (*SendStat_fn)     (char*, int, void*);
    typedef int (*ControlledExit_fn)(int, bool, bool, int, void*);
    typedef int (*SendData_fn)     (void*, int, int, void*);
    typedef int (*SendInitData_fn) (void*, int, void*);
    typedef int (*BGThreadRunning_fn)(bool, int, void*);

    typedef int  (*ng_Init_t)(SendChar_fn, SendStat_fn, ControlledExit_fn,
                              SendData_fn, SendInitData_fn, BGThreadRunning_fn,
                              void*);
    typedef int  (*ng_Command_t)(char*);
    typedef vector_info * (*ng_GetVecInfo_t)(char*);
    typedef char ** (*ng_AllVecs_t)(char*);
    typedef char *  (*ng_CurPlot_t)(void);
    typedef bool (*ng_Running_t)(void);
}

std::mutex   g_mtx;
std::atomic<bool> g_ready { false };
bool         g_loaded  = false;
void *       g_dl      = nullptr;

ng_Init_t         ng_Init         = nullptr;
ng_Command_t      ng_Command      = nullptr;
ng_GetVecInfo_t   ng_GetVecInfo   = nullptr;
ng_AllVecs_t      ng_AllVecs      = nullptr;
ng_CurPlot_t      ng_CurPlot      = nullptr;
ng_Running_t      ng_Running      = nullptr;

std::string       g_version;
thread_local std::string tls_log;

// ngspice callbacks. `caller` is our RunResult*, but we accept anything.
extern "C" int send_char_cb(char * s, int /*id*/, void * /*p*/) {
    if (s) { tls_log += s; tls_log += '\n'; }
    return 0;
}
extern "C" int send_stat_cb(char * /*s*/, int /*id*/, void * /*p*/) { return 0; }
extern "C" int ctrl_exit_cb(int /*status*/, bool /*imm*/, bool /*quit*/,
                            int /*id*/, void * /*p*/) { return 0; }
extern "C" int send_data_cb(void * /*d*/, int /*n*/, int /*id*/, void * /*p*/) { return 0; }
extern "C" int send_init_cb(void * /*d*/, int /*id*/, void * /*p*/) { return 0; }
extern "C" int bg_running_cb(bool /*not_running*/, int /*id*/, void * /*p*/) { return 0; }

bool try_load() {
    const char * candidates[] = {
        "libngspice.so.0",
        "libngspice.so",
        "libngspice.so.30",
        nullptr
    };
    for (int i = 0; candidates[i]; ++i) {
        void * h = ::dlopen(candidates[i], RTLD_NOW | RTLD_LOCAL);
        if (h) { g_dl = h; break; }
    }
    if (!g_dl) return false;

    auto sym = [](const char * name) {
        return ::dlsym(g_dl, name);
    };
    ng_Init        = reinterpret_cast<ng_Init_t>       (sym("ngSpice_Init"));
    ng_Command     = reinterpret_cast<ng_Command_t>    (sym("ngSpice_Command"));
    ng_GetVecInfo  = reinterpret_cast<ng_GetVecInfo_t> (sym("ngGet_Vec_Info"));
    ng_AllVecs     = reinterpret_cast<ng_AllVecs_t>    (sym("ngSpice_AllVecs"));
    ng_CurPlot     = reinterpret_cast<ng_CurPlot_t>    (sym("ngSpice_CurPlot"));
    ng_Running     = reinterpret_cast<ng_Running_t>    (sym("ngSpice_running"));

    if (!ng_Init || !ng_Command || !ng_GetVecInfo || !ng_AllVecs) {
        ::dlclose(g_dl); g_dl = nullptr;
        return false;
    }

    tls_log.clear();
    int rc = ng_Init(&send_char_cb, &send_stat_cb, &ctrl_exit_cb,
                     &send_data_cb, &send_init_cb, &bg_running_cb,
                     nullptr);
    (void) rc;

    // Best-effort version string via `version` command.
    tls_log.clear();
    char cmd[] = "version";
    ng_Command(cmd);
    g_version = tls_log;
    while (!g_version.empty() && (g_version.back() == '\n' || g_version.back() == ' '))
        g_version.pop_back();

    g_loaded = true;
    return true;
}

} // namespace

bool available() { return g_loaded; }
std::string version() { return g_version; }

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_ready) return;
    if (!g_loaded) try_load();
    g_ready = true;
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_dl) { ::dlclose(g_dl); g_dl = nullptr; }
    g_loaded = false;
    g_ready  = false;
}

Status status() {
    Status s;
    s.ready  = g_ready;
    if (g_loaded) s.detail = "libngspice loaded" + (g_version.empty() ? std::string{} : (" (" + g_version + ")"));
    else          s.detail = "libngspice not available";
    return s;
}

RunResult run(std::string_view netlist, std::string_view analysis_command) {
    RunResult r;
    if (!g_ready) init();
    if (!g_loaded) {
        r.error = "libngspice not loaded; install libngspice0";
        return r;
    }

    tls_log.clear();

    // 1. Free any previous circuit.
    { char c[] = "destroy all"; ng_Command(c); }

    // 2. Load the netlist via `circbyline` line-by-line. This mirrors
    // eeschema's own SPICE bridge and works with ngspice-shared.
    std::size_t start = 0;
    while (start < netlist.size()) {
        std::size_t nl = netlist.find('\n', start);
        std::string line(netlist.substr(start, nl == std::string_view::npos ? netlist.size() - start : nl - start));
        while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
        std::string cmd = std::string("circbyline ") + line;
        std::vector<char> buf(cmd.begin(), cmd.end());
        buf.push_back('\0');
        ng_Command(buf.data());
        if (nl == std::string_view::npos) break;
        start = nl + 1;
    }

    // 3. Run the analysis.
    {
        std::string cmd(analysis_command);
        std::vector<char> buf(cmd.begin(), cmd.end());
        buf.push_back('\0');
        ng_Command(buf.data());
    }

    // 4. Collect vectors.
    char * cp = ng_CurPlot ? ng_CurPlot() : nullptr;
    (void) cp;
    char ** vecs = ng_AllVecs(cp);
    if (vecs) {
        for (int i = 0; vecs[i]; ++i) {
            vector_info * vi = ng_GetVecInfo(vecs[i]);
            if (!vi) continue;
            Signal s;
            s.name = vi->v_name ? vi->v_name : "";
            s.is_complex = vi->v_compdata != nullptr;
            s.values.reserve(static_cast<std::size_t>(std::max(0, vi->v_length)));
            if (vi->v_realdata) {
                for (int k = 0; k < vi->v_length; ++k) s.values.push_back(vi->v_realdata[k]);
            }
            r.signals.push_back(std::move(s));
        }
    }

    r.log = tls_log;
    r.ok  = !r.signals.empty();
    if (!r.ok && r.error.empty()) r.error = "ngspice returned no vectors";
    return r;
}

RunResult run_file(std::string_view netlist_path, std::string_view analysis_command) {
    std::ifstream f{std::string(netlist_path)};
    if (!f) { RunResult r; r.error = "cannot read netlist file"; return r; }
    std::stringstream ss; ss << f.rdbuf();
    return run(ss.str(), analysis_command);
}

std::string sample_rc_netlist() {
    return
        ".title RC lowpass smoke test\n"
        "V1 in 0 PULSE(0 5 0 1n 1n 500u 1m)\n"
        "R1 in out 1k\n"
        "C1 out 0 100n\n"
        ".end\n";
}

} // namespace spice_simulator
