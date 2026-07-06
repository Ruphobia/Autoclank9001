// SPDX-License-Identifier: GPL-3.0-or-later
#include "coder.hpp"

#include "../../010_interface/status.hpp"

#include "llama.h"
#include "../../model_chunks.hpp"

extern "C" void physics_shutdown_if_loaded();
extern "C" void chemistry_shutdown_if_loaded();
extern "C" void vision_shutdown_if_loaded();
extern "C" void planner_shutdown_if_loaded();

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace coder {
namespace {

constexpr const char * kModelRelPath =
    "resources/models/coder/Qwen2.5-Coder-14B-Instruct-abliterated.Q5_K_M.gguf";

// Pin to second physical P100. The first card holds cleanup (1.5B) + the
// understanding qwen14b stack (10 GB). This card holds the coder alone.
constexpr int kMainGpu = 1;

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

Runtime * get_runtime_locked() {
    if (g_runtime) return g_runtime;

    // Evict any other tenant of GPU 1 before we load.
    physics_shutdown_if_loaded();
    chemistry_shutdown_if_loaded();
    vision_shutdown_if_loaded();
    planner_shutdown_if_loaded();

    llama_model_params mp = llama_model_default_params();
    mp.n_gpu_layers = 999;
    mp.split_mode   = LLAMA_SPLIT_MODE_LAYER;
    mp.main_gpu     = kMainGpu;
    mp.use_mmap     = true;

    if (!model_chunks::ensure(kModelRelPath)) {
        throw std::runtime_error(
            std::string("coder: model file missing and chunks not found: ") + kModelRelPath);
    }
    status::loading_set("coder");
    llama_model * model = llama_model_load_from_file(kModelRelPath, mp);
    if (!model) {
        throw std::runtime_error(
            std::string("coder: failed to load GGUF: ") + kModelRelPath);
    }

    llama_context_params cp = llama_context_default_params();
    // Bumped from 8192 to 16384 so the coder has room for its base
    // system prompt (~4500 tok), header context injected by the ticket
    // runner (up to ~1500 tok), the ticket body (~500 tok), an optional
    // planner-injected plan (up to ~1500 tok), and still has 5632 tok
    // free for its own reply. Adds ~1.5 GB of KV cache; both P100s have
    // room.
    cp.n_ctx           = 16384;
    // n_batch must be >= the largest single prompt we decode in one shot,
    // otherwise llama_decode asserts (n_tokens_all <= n_batch) and aborts
    // the process. Keep it equal to n_ctx so any prompt that fits the
    // context also fits one batch.
    cp.n_batch         = 16384;
    cp.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_DISABLED;

    llama_context * ctx = llama_init_from_model(model, cp);
    if (!ctx) {
        llama_model_free(model);
        throw std::runtime_error("coder: llama_init_from_model failed");
    }

    status::loading_clear();
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
}

}  // namespace coder

// Cross-shutdown handshake — called by physics::get_runtime_locked() before
// it loads. The function name is at file scope (no namespace) so it pairs
// with the matching extern "C" declared in physics.hpp.
extern "C" void coder_shutdown_if_loaded() {
    coder::shutdown();
}

namespace coder {

std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int max_new_tokens,
                     bool * truncated) {
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
        throw std::runtime_error("coder: chat template apply failed");
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
        throw std::runtime_error("coder: tokenize sizing failed");
    }
    std::vector<llama_token> toks(static_cast<std::size_t>(needed));
    if (llama_tokenize(
            vocab, prompt.c_str(), static_cast<int>(prompt.size()),
            toks.data(), static_cast<int>(toks.size()),
            is_first, /*parse_special=*/true) < 0) {
        throw std::runtime_error("coder: tokenize failed");
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

    for (int produced = 0; produced < max_new_tokens; ) {
        const uint32_t used = static_cast<uint32_t>(
            llama_memory_seq_pos_max(llama_get_memory(ctx), 0) + 1);
        if (used + static_cast<uint32_t>(batch.n_tokens) > ctx_cap) {
            std::fprintf(stderr,
                "coder: context full at %u/%u; truncating output\n",
                used, ctx_cap);
            break;
        }
        const int rc = llama_decode(ctx, batch);
        if (rc != 0) {
            std::fprintf(stderr,
                "coder: llama_decode rc=%d; aborting\n", rc);
            break;
        }
        if (produced % 50 == 0) status::progress_set("coder", produced, max_new_tokens);
        status::pulse();
        if (status::generation_cancelled()) break;

        new_id = llama_sampler_sample(smpl, ctx, /*idx=*/-1);
        if (llama_vocab_is_eog(vocab, new_id)) { hit_eog = true; break; }

        char piece[256];
        const int n = llama_token_to_piece(
            vocab, new_id, piece, static_cast<int>(sizeof(piece)),
            /*lstrip=*/0, /*special=*/false);
        if (n < 0) {
            std::fprintf(stderr, "coder: token_to_piece failed\n");
            break;
        }
        out.append(piece, piece + n);
        ++produced;

        batch = llama_batch_get_one(&new_id, 1);
    }

    status::progress_clear();
    llama_sampler_free(smpl);

    // Any exit without an end-of-generation token (context full, decode
    // error, max_new_tokens) means the text is an incomplete prefix.
    if (truncated) *truncated = !hit_eog;

    return strip(out);
}

}
