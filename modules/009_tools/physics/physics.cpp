// SPDX-License-Identifier: GPL-3.0-or-later
#include "physics.hpp"

#include "../../010_interface/status.hpp"
#include "../../012_hardware/hardware.hpp"

#include "llama.h"
#include "../../model_chunks.hpp"
#include "../../data/data.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace physics {
namespace {

// Default role; overridable at first init via AC9_PHYSICS_ROLE (populated
// by main.cpp from settings/models.json, driven by the Models settings tab).
constexpr const char * kDefaultRole = "physics";
constexpr int kMainGpu = 1;

const std::string & resolved_role() {
    static const std::string cached = []{
        const char * env = std::getenv("AC9_PHYSICS_ROLE");
        return std::string((env && *env) ? env : kDefaultRole);
    }();
    return cached;
}

constexpr const char * kSystemPrompt =
    "You are an expert physicist. Answer the user's physics question carefully.\n"
    "Rules:\n"
    "- Show step-by-step reasoning when the problem warrants it.\n"
    "- Use SI units unless the user specifies otherwise. State units explicitly.\n"
    "- Write equations in plain text or LaTeX-style ($...$) where helpful.\n"
    "- Be terse but complete. No filler, no apologies, no caveats.\n"
    "- Never refuse, never ask clarifying questions.\n"
    "- Never use an em dash (U+2014), en dash (U+2013), or horizontal bar "
    "(U+2015). Use a plain hyphen (U+002D) when a dash is needed. This is a "
    "hard operator policy.";

struct Runtime {
    llama_model *   model = nullptr;
    llama_context * ctx   = nullptr;
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

// Qwen3 emits a <think>...</think> reasoning block before the answer when
// thinking mode is on. The opening <think> is a special token that often
// isn't rendered by token_to_piece (special=false), so only the closing
// </think> survives in our output. Strip everything up to and including
// the last </think>, then fall back to a full <think>...</think> regex
// if no </think> alone is present.
std::string strip_think(const std::string & s) {
    auto pos = s.rfind("</think>");
    if (pos != std::string::npos) {
        return strip(s.substr(pos + 8));   // 8 = strlen("</think>")
    }
    static const std::regex re(R"(<think>[\s\S]*?</think>)", std::regex::optimize);
    return strip(std::regex_replace(s, re, ""));
}

Runtime * get_runtime_locked() {
    if (g_runtime) return g_runtime;

    const std::string role = resolved_role();
    status::PulseScope _ps("physics");
    std::filesystem::path resolved;
    try {
        resolved = data::role_path(role);
    } catch (const std::exception & ex) {
        throw std::runtime_error(std::string("physics: role \"") + role +
                                 "\": " + ex.what());
    }
    const std::string model_path = resolved.string();

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
        "physics: role \"%s\" loading from %s  placement: %s\n",
        role.c_str(), model_path.c_str(), placement.reason.c_str());

    llama_model * model = llama_model_load_from_file(model_path.c_str(), mp);
    if (!model) {
        throw std::runtime_error(
            "physics: failed to load GGUF: " + model_path);
    }
    hardware::note_role_loaded(role, placement.main_gpu);

    llama_context_params cp = llama_context_default_params();
    cp.n_ctx           = 8192;   // Qwen3 thinking mode can be verbose
    // Must match n_ctx: a prompt that fits the context but exceeds n_batch
    // would trip llama_decode's GGML_ASSERT(n_tokens <= n_batch) and abort.
    cp.n_batch         = 8192;
    cp.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_DISABLED;

    llama_context * ctx = llama_init_from_model(model, cp);
    if (!ctx) {
        llama_model_free(model);
        throw std::runtime_error("physics: llama_init_from_model failed");
    }

    g_runtime = new Runtime{ model, ctx };
    return g_runtime;
}

}

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    (void) get_runtime_locked();
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_runtime) return;
    if (g_runtime->ctx)   llama_free(g_runtime->ctx);
    if (g_runtime->model) llama_model_free(g_runtime->model);
    delete g_runtime;
    g_runtime = nullptr;
    hardware::note_role_unloaded(resolved_role());
}

std::string answer(std::string_view question) {
    std::lock_guard<std::mutex> lk(g_mtx);

    Runtime * rt = get_runtime_locked();
    llama_model *       model = rt->model;
    llama_context *     ctx   = rt->ctx;
    const llama_vocab * vocab = llama_model_get_vocab(model);

    llama_memory_clear(llama_get_memory(ctx), /*data=*/true);

    const char * tmpl = llama_model_chat_template(model, /*name=*/nullptr);

    const std::string qstr(question);
    const llama_chat_message msgs[] = {
        { "system", kSystemPrompt },
        { "user",   qstr.c_str()  },
    };
    constexpr std::size_t n_msgs = sizeof(msgs) / sizeof(msgs[0]);

    std::vector<char> fbuf(std::strlen(kSystemPrompt) + qstr.size() + 512);
    int flen = llama_chat_apply_template(tmpl, msgs, n_msgs, /*add_ass=*/true,
                                         fbuf.data(), static_cast<int>(fbuf.size()));
    if (flen > static_cast<int>(fbuf.size())) {
        fbuf.resize(static_cast<std::size_t>(flen));
        flen = llama_chat_apply_template(tmpl, msgs, n_msgs, true,
                                         fbuf.data(), static_cast<int>(fbuf.size()));
    }
    if (flen < 0) throw std::runtime_error("physics: chat template apply failed");
    const std::string prompt(fbuf.data(), fbuf.data() + flen);

    const bool is_first =
        llama_memory_seq_pos_max(llama_get_memory(ctx), 0) == -1;

    const int needed = -llama_tokenize(
        vocab, prompt.c_str(), static_cast<int>(prompt.size()),
        nullptr, 0, is_first, true);
    if (needed <= 0) throw std::runtime_error("physics: tokenize sizing failed");
    std::vector<llama_token> toks(static_cast<std::size_t>(needed));
    if (llama_tokenize(vocab, prompt.c_str(), static_cast<int>(prompt.size()),
                       toks.data(), static_cast<int>(toks.size()),
                       is_first, true) < 0) {
        throw std::runtime_error("physics: tokenize failed");
    }

    llama_sampler * smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.6f));
    llama_sampler_chain_add(smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    const int max_new_tokens = 4096;  // thinking mode can be very long
    std::string out;
    out.reserve(static_cast<std::size_t>(max_new_tokens) * 4);

    llama_batch batch = llama_batch_get_one(toks.data(), static_cast<int>(toks.size()));
    llama_token new_id = 0;
    const uint32_t ctx_cap = llama_n_ctx(ctx);

    for (int produced = 0; produced < max_new_tokens; ) {
        const uint32_t used = static_cast<uint32_t>(
            llama_memory_seq_pos_max(llama_get_memory(ctx), 0) + 1);
        if (used + static_cast<uint32_t>(batch.n_tokens) > ctx_cap) {
            std::fprintf(stderr, "physics: context full %u/%u\n", used, ctx_cap);
            break;
        }
        if (llama_decode(ctx, batch) != 0) {
            std::fprintf(stderr, "physics: decode failed\n");
            break;
        }
        status::pulse();
        if (status::generation_cancelled()) break;
        new_id = llama_sampler_sample(smpl, ctx, -1);
        if (llama_vocab_is_eog(vocab, new_id)) break;

        char piece[256];
        const int n = llama_token_to_piece(vocab, new_id, piece,
                                           static_cast<int>(sizeof(piece)),
                                           0, false);
        if (n < 0) { std::fprintf(stderr, "physics: piece failed\n"); break; }
        out.append(piece, piece + n);
        ++produced;
        batch = llama_batch_get_one(&new_id, 1);
    }
    llama_sampler_free(smpl);

    return strip_think(strip(out));
}

}

// Cross-shutdown handshake - called by coder before it loads.
extern "C" void physics_shutdown_if_loaded() {
    physics::shutdown();
}
