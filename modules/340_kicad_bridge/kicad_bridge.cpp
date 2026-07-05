// SPDX-License-Identifier: GPL-3.0-or-later
#include "kicad_bridge.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern char ** environ;

namespace kicad_bridge {
namespace {

std::mutex g_mtx;
Config     g_cfg;
bool       g_ready = false;

bool file_executable(const std::string & p) {
    if (p.empty()) return false;
    struct stat st{};
    if (::stat(p.c_str(), &st) != 0) return false;
    if (!S_ISREG(st.st_mode)) return false;
    return (::access(p.c_str(), X_OK) == 0);
}

std::string first_line(const std::string & s) {
    auto nl = s.find('\n');
    return nl == std::string::npos ? s : s.substr(0, nl);
}

// Discover a kicad-cli. Priority: TOOL_KICAD_CLI env, then the locally
// built one under ~/work/kicad/build/kicad, then $PATH via `which`,
// then the two common install prefixes.
std::string discover_cli() {
    if (const char * env = std::getenv("TOOL_KICAD_CLI")) {
        if (file_executable(env)) return env;
    }
    const char * home = std::getenv("HOME");
    if (home) {
        std::string local = std::string(home) + "/work/kicad/build/kicad/kicad-cli";
        if (file_executable(local)) return local;
    }
    // Try $PATH via a tiny popen; avoids duplicating $PATH split logic.
    {
        FILE * p = ::popen("command -v kicad-cli 2>/dev/null", "r");
        if (p) {
            char buf[512];
            std::string out;
            while (auto n = std::fread(buf, 1, sizeof(buf), p)) {
                out.append(buf, buf + n);
            }
            ::pclose(p);
            while (!out.empty() && (out.back() == '\n' || out.back() == '\r'))
                out.pop_back();
            if (file_executable(out)) return out;
        }
    }
    for (const char * candidate : {"/usr/local/bin/kicad-cli", "/usr/bin/kicad-cli"}) {
        if (file_executable(candidate)) return candidate;
    }
    return {};
}

// Try to locate a stock-data directory alongside the CLI so KiCad stops
// complaining about api.v1.schema.json on every invocation. This is
// best-effort; unavailable is fine.
std::string discover_stock_data(const std::string & cli) {
    if (cli.empty()) return {};
    // Sibling to the build tree: ~/work/kicad/resources/schemas etc.
    // Or install prefix: /usr/local/share/kicad or /usr/share/kicad.
    for (const char * s : {"/usr/local/share/kicad", "/usr/share/kicad"}) {
        struct stat st{};
        if (::stat(s, &st) == 0 && S_ISDIR(st.st_mode)) return s;
    }
    return {};
}

// A tiny drain-both-pipes exec wrapper. Not fancy, no async, but does
// return both streams and the real exit code (unlike popen which merges
// and unlike system() which discards output).
struct ExecCap {
    int         exit_code;
    std::string out;
    std::string err;
};

ExecCap exec_capture(const std::vector<std::string> & argv_in,
                     const std::string & cwd,
                     const std::string & input,
                     const std::string & env_prefix /*e.g. "KICAD_STOCK_DATA_HOME=..."*/) {
    ExecCap r{-1, {}, {}};
    if (argv_in.empty()) {
        r.err = "exec: empty argv";
        return r;
    }
    int outp[2]{-1, -1}, errp[2]{-1, -1}, inp[2]{-1, -1};
    if (::pipe(outp) || ::pipe(errp) || ::pipe(inp)) {
        r.err = std::string("pipe: ") + std::strerror(errno);
        return r;
    }

    pid_t pid = ::fork();
    if (pid < 0) {
        r.err = std::string("fork: ") + std::strerror(errno);
        for (int fd : {outp[0], outp[1], errp[0], errp[1], inp[0], inp[1]}) if (fd >= 0) ::close(fd);
        return r;
    }
    if (pid == 0) {
        // Child.
        ::dup2(inp[0],  STDIN_FILENO);
        ::dup2(outp[1], STDOUT_FILENO);
        ::dup2(errp[1], STDERR_FILENO);
        for (int fd : {outp[0], outp[1], errp[0], errp[1], inp[0], inp[1]}) ::close(fd);
        if (!cwd.empty()) {
            if (::chdir(cwd.c_str()) != 0) {
                std::fprintf(stderr, "chdir(%s): %s\n", cwd.c_str(), std::strerror(errno));
                std::_Exit(126);
            }
        }
        // Apply env prefix. Format: "K=V".
        if (!env_prefix.empty()) {
            auto eq = env_prefix.find('=');
            if (eq != std::string::npos) {
                ::setenv(env_prefix.substr(0, eq).c_str(),
                         env_prefix.substr(eq + 1).c_str(), 1);
            }
        }
        std::vector<char *> argv;
        argv.reserve(argv_in.size() + 1);
        for (const auto & s : argv_in) argv.push_back(const_cast<char *>(s.c_str()));
        argv.push_back(nullptr);
        ::execve(argv_in[0].c_str(), argv.data(), environ);
        std::fprintf(stderr, "execve(%s): %s\n",
                     argv_in[0].c_str(), std::strerror(errno));
        std::_Exit(127);
    }

    // Parent.
    ::close(outp[1]); ::close(errp[1]); ::close(inp[0]);
    if (!input.empty()) {
        ssize_t total = 0, want = static_cast<ssize_t>(input.size());
        while (total < want) {
            ssize_t n = ::write(inp[1], input.data() + total, want - total);
            if (n <= 0) break;
            total += n;
        }
    }
    ::close(inp[1]);

    auto drain = [](int fd, std::string & sink) {
        char buf[4096];
        for (;;) {
            ssize_t n = ::read(fd, buf, sizeof(buf));
            if (n > 0) sink.append(buf, buf + n);
            else if (n == 0) return;
            else if (errno == EINTR) continue;
            else return;
        }
    };
    drain(outp[0], r.out);
    drain(errp[0], r.err);
    ::close(outp[0]); ::close(errp[0]);

    int status = 0;
    while (::waitpid(pid, &status, 0) < 0 && errno == EINTR) {}
    if (WIFEXITED(status))         r.exit_code = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))  r.exit_code = 128 + WTERMSIG(status);
    return r;
}

RunResult ensure_ready() {
    RunResult rr;
    if (!g_ready) {
        rr.stderr_text = "kicad_bridge not initialized";
        return rr;
    }
    if (!g_cfg.available) {
        rr.stderr_text = "kicad-cli not found; set TOOL_KICAD_CLI or install kicad";
        return rr;
    }
    return rr;
}

} // namespace

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_ready) return;
    g_cfg.cli_path = discover_cli();
    g_cfg.stock_data_home = discover_stock_data(g_cfg.cli_path);
    if (!g_cfg.cli_path.empty()) {
        std::string env_prefix;
        if (!g_cfg.stock_data_home.empty())
            env_prefix = "KICAD_STOCK_DATA_HOME=" + g_cfg.stock_data_home;
        auto v = exec_capture({g_cfg.cli_path, "version"}, {}, {}, env_prefix);
        if (v.exit_code == 0) {
            g_cfg.version   = first_line(v.out);
            g_cfg.available = true;
        }
    }
    g_ready = true;
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_cfg   = {};
    g_ready = false;
}

const Config & config() { return g_cfg; }

RunResult run(const std::vector<std::string> & args,
              std::string_view cwd,
              std::string_view input) {
    RunResult check = ensure_ready();
    if (!check.stderr_text.empty()) return check;

    std::vector<std::string> full;
    full.reserve(args.size() + 1);
    full.push_back(g_cfg.cli_path);
    for (const auto & a : args) full.push_back(a);

    std::string env_prefix;
    if (!g_cfg.stock_data_home.empty())
        env_prefix = "KICAD_STOCK_DATA_HOME=" + g_cfg.stock_data_home;

    auto e = exec_capture(full, std::string(cwd), std::string(input), env_prefix);
    RunResult rr;
    rr.exit_code   = e.exit_code;
    rr.stdout_text = std::move(e.out);
    rr.stderr_text = std::move(e.err);
    return rr;
}

// --- Schematic ------------------------------------------------------

RunResult sch_erc(std::string_view sch_path, std::string_view report_out,
                  bool json_format) {
    std::vector<std::string> args = {
        "sch", "erc",
        "--output", std::string(report_out),
        "--format", json_format ? "json" : "report",
        "--severity-all",
        std::string(sch_path)
    };
    auto rr = run(args);
    if (rr.exit_code >= 0 && !report_out.empty()) rr.output_path = std::string(report_out);
    return rr;
}

RunResult sch_netlist(std::string_view sch_path, std::string_view net_out,
                      std::string_view format) {
    std::vector<std::string> args = {
        "sch", "export", "netlist",
        "--output", std::string(net_out),
        "--format", std::string(format),
        std::string(sch_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(net_out);
    return rr;
}

RunResult sch_export_svg(std::string_view sch_path, std::string_view dir_out) {
    std::vector<std::string> args = {
        "sch", "export", "svg",
        "--output", std::string(dir_out),
        std::string(sch_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(dir_out);
    return rr;
}

RunResult sch_export_pdf(std::string_view sch_path, std::string_view pdf_out) {
    std::vector<std::string> args = {
        "sch", "export", "pdf",
        "--output", std::string(pdf_out),
        std::string(sch_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(pdf_out);
    return rr;
}

RunResult sch_export_bom(std::string_view sch_path, std::string_view csv_out,
                         std::string_view fields) {
    std::vector<std::string> args = {
        "sch", "export", "bom",
        "--output", std::string(csv_out),
        "--fields", std::string(fields),
        std::string(sch_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(csv_out);
    return rr;
}

// --- PCB ------------------------------------------------------------

RunResult pcb_drc(std::string_view pcb_path, std::string_view report_out,
                  bool json_format, bool schematic_parity) {
    std::vector<std::string> args = {
        "pcb", "drc",
        "--output", std::string(report_out),
        "--format", json_format ? "json" : "report",
        "--severity-all"
    };
    if (schematic_parity) args.emplace_back("--schematic-parity");
    args.emplace_back(std::string(pcb_path));
    auto rr = run(args);
    if (rr.exit_code >= 0 && !report_out.empty()) rr.output_path = std::string(report_out);
    return rr;
}

RunResult pcb_export_gerbers(std::string_view pcb_path,
                             std::string_view dir_out,
                             const std::vector<std::string> & layers) {
    std::vector<std::string> args = {
        "pcb", "export", "gerbers",
        "--output", std::string(dir_out)
    };
    if (!layers.empty()) {
        std::string joined;
        for (std::size_t i = 0; i < layers.size(); ++i) {
            if (i) joined += ",";
            joined += layers[i];
        }
        args.emplace_back("--layers");
        args.emplace_back(std::move(joined));
    }
    args.emplace_back(std::string(pcb_path));
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(dir_out);
    return rr;
}

RunResult pcb_export_drill(std::string_view pcb_path, std::string_view dir_out) {
    std::vector<std::string> args = {
        "pcb", "export", "drill",
        "--output", std::string(dir_out),
        std::string(pcb_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(dir_out);
    return rr;
}

RunResult pcb_export_step(std::string_view pcb_path, std::string_view step_out) {
    std::vector<std::string> args = {
        "pcb", "export", "step",
        "--output", std::string(step_out),
        std::string(pcb_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(step_out);
    return rr;
}

RunResult pcb_export_svg(std::string_view pcb_path, std::string_view svg_out,
                         const std::vector<std::string> & layers) {
    std::vector<std::string> args = {
        "pcb", "export", "svg",
        "--output", std::string(svg_out)
    };
    if (!layers.empty()) {
        std::string joined;
        for (std::size_t i = 0; i < layers.size(); ++i) {
            if (i) joined += ",";
            joined += layers[i];
        }
        args.emplace_back("--layers");
        args.emplace_back(std::move(joined));
    }
    args.emplace_back(std::string(pcb_path));
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(svg_out);
    return rr;
}

RunResult pcb_export_pos(std::string_view pcb_path, std::string_view csv_out,
                         std::string_view side, std::string_view fmt) {
    std::vector<std::string> args = {
        "pcb", "export", "pos",
        "--output", std::string(csv_out),
        "--side", std::string(side),
        "--format", std::string(fmt),
        std::string(pcb_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(csv_out);
    return rr;
}

RunResult pcb_render_png(std::string_view pcb_path, std::string_view png_out,
                         int width_px, int height_px, std::string_view side) {
    std::vector<std::string> args = {
        "pcb", "render",
        "--output", std::string(png_out),
        "--width",  std::to_string(width_px),
        "--height", std::to_string(height_px),
        "--side",   std::string(side),
        std::string(pcb_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(png_out);
    return rr;
}

RunResult fp_export_svg(std::string_view lib_path,
                        std::string_view footprint_name,
                        std::string_view svg_out) {
    std::vector<std::string> args = {
        "fp", "export", "svg",
        "--output", std::string(svg_out),
        "--footprint", std::string(footprint_name),
        std::string(lib_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(svg_out);
    return rr;
}

RunResult sym_export_svg(std::string_view lib_path,
                         std::string_view symbol_name,
                         std::string_view svg_out) {
    std::vector<std::string> args = {
        "sym", "export", "svg",
        "--output", std::string(svg_out),
        "--symbol", std::string(symbol_name),
        std::string(lib_path)
    };
    auto rr = run(args);
    if (rr.exit_code == 0) rr.output_path = std::string(svg_out);
    return rr;
}

} // namespace kicad_bridge
