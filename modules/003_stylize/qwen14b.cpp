// SPDX-License-Identifier: GPL-3.0-or-later
#include "qwen14b.hpp"

#include "../010_interface/status.hpp"

#include "llama.h"
#include "../model_chunks.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace qwen14b {
namespace {

constexpr const char * kModelRelPath =
    "resources/models/qwen14b/Qwen2.5-14B-Instruct-abliterated.Q5_K_M.gguf";

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

    llama_model_params mp = llama_model_default_params();
    mp.n_gpu_layers = 999;
    mp.split_mode   = LLAMA_SPLIT_MODE_NONE;
    mp.main_gpu     = 0;
    mp.use_mmap     = true;

    if (!model_chunks::ensure(kModelRelPath)) {
        throw std::runtime_error(
            std::string("qwen14b: model file missing and chunks not found: ") + kModelRelPath);
    }
    llama_model * model = llama_model_load_from_file(kModelRelPath, mp);
    if (!model) {
        throw std::runtime_error(
            std::string("qwen14b: failed to load GGUF: ") + kModelRelPath);
    }

    llama_context_params cp = llama_context_default_params();
    cp.n_ctx           = 4096;
    cp.n_batch         = 4096;
    cp.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_DISABLED;

    llama_context * ctx = llama_init_from_model(model, cp);
    if (!ctx) {
        llama_model_free(model);
        throw std::runtime_error("qwen14b: llama_init_from_model failed");
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
}

std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int max_new_tokens) {
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
        throw std::runtime_error("qwen14b: chat template apply failed");
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
        throw std::runtime_error("qwen14b: tokenize sizing failed");
    }
    std::vector<llama_token> toks(static_cast<std::size_t>(needed));
    if (llama_tokenize(
            vocab, prompt.c_str(), static_cast<int>(prompt.size()),
            toks.data(), static_cast<int>(toks.size()),
            is_first, /*parse_special=*/true) < 0) {
        throw std::runtime_error("qwen14b: tokenize failed");
    }

    llama_sampler * smpl =
        llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

    std::string out;
    out.reserve(static_cast<std::size_t>(max_new_tokens) * 4);

    llama_batch    batch  = llama_batch_get_one(toks.data(), static_cast<int>(toks.size()));
    llama_token    new_id = 0;
    const uint32_t ctx_cap = llama_n_ctx(ctx);

    for (int produced = 0; produced < max_new_tokens; ) {
        const uint32_t used = static_cast<uint32_t>(
            llama_memory_seq_pos_max(llama_get_memory(ctx), 0) + 1);
        if (used + static_cast<uint32_t>(batch.n_tokens) > ctx_cap) {
            std::fprintf(stderr,
                "qwen14b: context full at %u/%u; truncating output\n",
                used, ctx_cap);
            break;
        }
        const int rc = llama_decode(ctx, batch);
        if (rc != 0) {
            std::fprintf(stderr,
                "qwen14b: llama_decode rc=%d; aborting\n", rc);
            break;
        }
        status::pulse();
        if (status::generation_cancelled()) break;

        new_id = llama_sampler_sample(smpl, ctx, /*idx=*/-1);
        if (llama_vocab_is_eog(vocab, new_id)) break;

        char piece[256];
        const int n = llama_token_to_piece(
            vocab, new_id, piece, static_cast<int>(sizeof(piece)),
            /*lstrip=*/0, /*special=*/false);
        if (n < 0) {
            std::fprintf(stderr, "qwen14b: token_to_piece failed\n");
            break;
        }
        out.append(piece, piece + n);
        ++produced;

        batch = llama_batch_get_one(&new_id, 1);
    }

    llama_sampler_free(smpl);

    return strip(out);
}

}
