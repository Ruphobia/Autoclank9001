// SPDX-License-Identifier: GPL-3.0-or-later
#include "planner.hpp"

#include "../../010_interface/status.hpp"
#include "../../012_hardware/hardware.hpp"
#include "../../013_bench/bench.hpp"
#include "../../data/data.hpp"

#include "llama.h"

extern "C" void coder_shutdown_if_loaded();
extern "C" void physics_shutdown_if_loaded();
extern "C" void chemistry_shutdown_if_loaded();
extern "C" void vision_shutdown_if_loaded();
extern "C" void qwen14b_shutdown_if_loaded();

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace planner {
namespace {

// Second physical P100; shared with coder/physics/chemistry/vision via
// the cross-shutdown handshake.
constexpr int kMainGpu = 1;

// Ctx capacity has to accommodate a full <think> trace (typically
// 500-4000 tokens) plus the plan itself.
constexpr int kCtx     = 16384;
constexpr int kBatch   = 16384;

std::string current_role() {
    if (const char * r = std::getenv("AC9_PLANNER_ROLE"); r && *r) {
        return r;
    }
    return "planner-4b";
}

struct Runtime {
    std::string     role;
    llama_model *   model = nullptr;
    llama_context * ctx   = nullptr;
    int             main_gpu = 0;
};

std::mutex g_mtx;
Runtime *  g_runtime = nullptr;

std::string strip(const std::string & s) {
    auto is_ws = [](unsigned char c) {
        return c == ' ' || c == '\t' || c == '\n' ||
               c == '\r' || c == '\f' || c == '\v';
    };
    std::size_t b = 0, e = s.size();
    while (b < e && is_ws(static_cast<unsigned char>(s[b])))     ++b;
    while (e > b && is_ws(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

// Remove any <think>...</think> block. Handles the well-formed shape
// (which is what Qwen3-*-Thinking emits when generation finishes) plus
// two truncation modes: (a) generation cut off inside <think>, no
// closing tag - discard everything since it's just reasoning, (b) some
// variants emit <think>...</think>\n\n<answer>...</answer> - strip the
// answer tags too.
std::string strip_think(const std::string & s) {
    std::string t = s;
    // (a) well-formed block(s)
    {
        static const std::regex re(R"(<think>[\s\S]*?</think>\s*)");
        t = std::regex_replace(t, re, "");
    }
    // (b) unclosed <think> tail: keep only the text after the tag
    if (auto pos = t.find("<think>"); pos != std::string::npos) {
        std::fprintf(stderr,
            "planner: warning: unclosed <think> in output; discarding\n");
        t = t.substr(0, pos);
    }
    // (c) drop <answer>...</answer> wrappers if present
    {
        static const std::regex ans_open(R"(<answer>\s*)");
        static const std::regex ans_close(R"(\s*</answer>)");
        t = std::regex_replace(t, ans_open, "");
        t = std::regex_replace(t, ans_close, "");
    }
    return t;
}

Runtime * get_runtime_locked() {
    if (g_runtime) return g_runtime;

    const std::string role = current_role();
    status::PulseScope _ps(role);
    std::fprintf(stderr,
        "planner: role \"%s\" (AC9_PLANNER_ROLE override to swap)\n",
        role.c_str());

    std::string model_path;
    try {
        model_path = data::resolve_role(role).string();
    } catch (const std::exception & ex) {
        throw std::runtime_error(std::string("planner: ") + ex.what());
    }

    std::uint64_t bytes = 0;
    try { bytes = std::filesystem::file_size(model_path); } catch (...) {}
    auto placement = hardware::pick_placement(role, bytes);
    hardware::request_evict(placement.displaced_role);

    llama_model_params mp = llama_model_default_params();
    mp.n_gpu_layers = placement.n_gpu_layers < 0 ? 999 : placement.n_gpu_layers;
    mp.split_mode   = LLAMA_SPLIT_MODE_NONE;
    mp.main_gpu     = placement.main_gpu;
    mp.use_mmap     = placement.mmap;

    std::fprintf(stderr,
        "planner: loading role=\"%s\" bytes=%.1fg  placement: %s\n",
        role.c_str(), double(bytes)/double(1ULL<<30),
        placement.reason.c_str());

    llama_model * model;
    {
        bench::LoadScope _bl(role, placement.main_gpu,
                             placement.displaced_role);
        model = llama_model_load_from_file(model_path.c_str(), mp);
        if (!model) {
            _bl.cancel();
            throw std::runtime_error(
                std::string("planner: failed to load GGUF: ") + model_path);
        }
    }
    hardware::note_role_loaded(role, placement.main_gpu);

    llama_context_params cp = llama_context_default_params();
    cp.n_ctx           = kCtx;
    cp.n_batch         = kBatch;
    cp.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_DISABLED;

    llama_context * ctx = llama_init_from_model(model, cp);
    if (!ctx) {
        llama_model_free(model);
        throw std::runtime_error("planner: llama_init_from_model failed");
    }

    g_runtime = new Runtime{ role, model, ctx, placement.main_gpu };
    return g_runtime;
}

}  // namespace (anon)

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    (void) get_runtime_locked();
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_runtime) return;
    const std::string role = g_runtime->role;
    if (g_runtime->ctx)   llama_free(g_runtime->ctx);
    if (g_runtime->model) llama_model_free(g_runtime->model);
    delete g_runtime;
    g_runtime = nullptr;
    hardware::note_role_unloaded(role);
}

}  // namespace planner

extern "C" void planner_shutdown_if_loaded() {
    planner::shutdown();
}

namespace planner {

std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int              max_new_tokens,
                     bool           * truncated) {
    std::lock_guard<std::mutex> lk(g_mtx);

    Runtime * rt = get_runtime_locked();
    llama_model *       model = rt->model;
    llama_context *     ctx   = rt->ctx;
    const llama_vocab * vocab = llama_model_get_vocab(model);

    llama_memory_clear(llama_get_memory(ctx), /*data=*/true);

    const char * tmpl = llama_model_chat_template(model, /*name=*/nullptr);

    const std::string sys_str(system_prompt);
    const std::string usr_str(user_msg);
    const llama_chat_message msgs[] = {
        { "system", sys_str.c_str() },
        { "user",   usr_str.c_str() },
    };
    constexpr size_t n_msgs = sizeof(msgs) / sizeof(msgs[0]);

    std::vector<char> fbuf(sys_str.size() + usr_str.size() + 512);
    int flen = llama_chat_apply_template(
        tmpl, msgs, n_msgs, /*add_ass=*/true,
        fbuf.data(), static_cast<int>(fbuf.size()));
    if (flen > static_cast<int>(fbuf.size())) {
        fbuf.resize(static_cast<std::size_t>(flen));
        flen = llama_chat_apply_template(
            tmpl, msgs, n_msgs, /*add_ass=*/true,
            fbuf.data(), static_cast<int>(fbuf.size()));
    }
    if (flen < 0) {
        throw std::runtime_error("planner: chat template apply failed");
    }
    const std::string prompt(fbuf.data(), fbuf.data() + flen);

    const bool is_first =
        llama_memory_seq_pos_max(llama_get_memory(ctx), 0) == -1;

    const int needed = -llama_tokenize(
        vocab, prompt.c_str(), static_cast<int>(prompt.size()),
        nullptr, 0,
        /*add_special=*/is_first,
        /*parse_special=*/true);
    if (needed <= 0) {
        throw std::runtime_error("planner: tokenize sizing failed");
    }
    std::vector<llama_token> toks(static_cast<std::size_t>(needed));
    if (llama_tokenize(
            vocab, prompt.c_str(), static_cast<int>(prompt.size()),
            toks.data(), static_cast<int>(toks.size()),
            is_first, /*parse_special=*/true) < 0) {
        throw std::runtime_error("planner: tokenize failed");
    }

    llama_sampler * smpl =
        llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

    std::string out;
    out.reserve(static_cast<std::size_t>(max_new_tokens) * 4);

    llama_batch    batch  = llama_batch_get_one(toks.data(), static_cast<int>(toks.size()));
    llama_token    new_id = 0;
    const uint32_t ctx_cap = llama_n_ctx(ctx);
    bool           hit_eog = false;

    bench::GenScope _bg(current_role(), rt->main_gpu,
                        static_cast<std::uint64_t>(toks.size()));
    bool prefill_done = false;
    const auto gen_start = std::chrono::steady_clock::now();
    int produced = 0;
    for (; produced < max_new_tokens; ) {
        const uint32_t used = static_cast<uint32_t>(
            llama_memory_seq_pos_max(llama_get_memory(ctx), 0) + 1);
        if (used + static_cast<uint32_t>(batch.n_tokens) > ctx_cap) {
            std::fprintf(stderr,
                "planner: context full at %u/%u; truncating output\n",
                used, ctx_cap);
            break;
        }
        const int rc = llama_decode(ctx, batch);
        if (rc != 0) {
            std::fprintf(stderr,
                "planner: llama_decode rc=%d; aborting\n", rc);
            break;
        }
        if (!prefill_done) {
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - gen_start).count();
            _bg.set_prefill_ms(static_cast<std::uint64_t>(ms));
            prefill_done = true;
        }
        if (produced % 50 == 0) status::progress_set(current_role(), produced, max_new_tokens);
        status::pulse();
        if (status::generation_cancelled()) break;

        new_id = llama_sampler_sample(smpl, ctx, /*idx=*/-1);
        if (llama_vocab_is_eog(vocab, new_id)) { hit_eog = true; break; }

        char piece[256];
        const int n = llama_token_to_piece(
            vocab, new_id, piece, static_cast<int>(sizeof(piece)),
            /*lstrip=*/0, /*special=*/false);
        if (n < 0) {
            std::fprintf(stderr, "planner: token_to_piece failed\n");
            break;
        }
        out.append(piece, piece + n);
        ++produced;

        batch = llama_batch_get_one(&new_id, 1);
    }

    status::progress_clear();
    llama_sampler_free(smpl);
    _bg.set_output_tokens(static_cast<std::uint64_t>(produced));
    if (truncated) *truncated = !hit_eog;

    out = strip_think(out);

    // Operator policy: ac9 never emits em/en dashes in any generated
    // text. Strip before returning so no downstream consumer (shell
    // parser, system-prompt inclusion, SSE frame, WRITEFILE body) can
    // ever leak one.
    {
        std::string t;
        t.reserve(out.size());
        for (std::size_t i = 0; i < out.size(); ) {
            // U+2013 EN DASH         -> UTF-8 E2 80 93
            // U+2014 EM DASH         -> UTF-8 E2 80 94
            // U+2015 HORIZONTAL BAR  -> UTF-8 E2 80 95
            // A byte-level scan naturally catches doubled/tripled runs
            // ("---", "———") since each occurrence is
            // rewritten independently to U+002D.
            if (i + 2 < out.size()
                && static_cast<unsigned char>(out[i])     == 0xE2
                && static_cast<unsigned char>(out[i + 1]) == 0x80
                && (static_cast<unsigned char>(out[i + 2]) == 0x93
                 || static_cast<unsigned char>(out[i + 2]) == 0x94
                 || static_cast<unsigned char>(out[i + 2]) == 0x95)) {
                t.push_back('-');
                i += 3;
            } else {
                t.push_back(out[i]);
                ++i;
            }
        }
        out = std::move(t);
    }

    return strip(out);
}

}  // namespace planner
