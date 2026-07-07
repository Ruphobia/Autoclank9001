// SPDX-License-Identifier: GPL-3.0-or-later
#include "coder.hpp"

#include "../../010_interface/status.hpp"
#include "../../012_hardware/hardware.hpp"
#include "../../013_bench/bench.hpp"

#include "llama.h"
#include "../../model_chunks.hpp"
#include "../../data/data.hpp"

extern "C" void physics_shutdown_if_loaded();
extern "C" void chemistry_shutdown_if_loaded();
extern "C" void vision_shutdown_if_loaded();
extern "C" void planner_shutdown_if_loaded();
extern "C" void qwen14b_shutdown_if_loaded();

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace coder {
namespace {

constexpr const char * kCoder14BRelPath =
    "resources/models/coder/Qwen2.5-Coder-14B-Instruct-abliterated.Q5_K_M.gguf";
constexpr const char * kCoderBigRelPath =
    "resources/models/coder/coder-big.gguf";

// Env AC9_CODER_ROLE selects which coder loads. Frozen at first init()
// and returned by active_role() so the interface layer can label the
// widget consistently. Falls back to "coder" for compatibility.
const std::string & resolved_role() {
    static const std::string cached = []{
        const char * env = std::getenv("AC9_CODER_ROLE");
        if (env && *env) return std::string(env);
        // Consolidated: coding + thinking now run on the single qwen35 model
        // (Qwen3.6-35B-A3B), layer-split across both cards. Legacy coder/
        // coder-big paths remain reachable via AC9_CODER_ROLE for A/B.
        return std::string("qwen35");
    }();
    return cached;
}

// Legacy single-file coders load from a fixed resources/ path; the qwen35
// role is reassembled from data/ chunks via data::resolve_role().
bool role_uses_data_chunks(const std::string & r) {
    return r != "coder" && r != "coder-big";
}

const char * resolved_model_path() {
    const auto & r = resolved_role();
    if (r == "coder-big") return kCoderBigRelPath;
    return kCoder14BRelPath;
}

struct Runtime {
    llama_model *   model    = nullptr;
    llama_context * ctx      = nullptr;
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

Runtime * get_runtime_locked() {
    if (g_runtime) return g_runtime;

    const std::string & role       = resolved_role();
    const bool          data_role  = role_uses_data_chunks(role);

    // Cover the entire load window (chunk reassembly + sibling eviction +
    // llama_model_load_from_file + llama_init_from_model) with one
    // pulse-thread so the client's rainbow never freezes.
    status::PulseScope _ps(role);

    // Resolve the model file. qwen35 (and any other data/-chunked role) is
    // reassembled + sha-verified from data/ chunks; legacy coder/coder-big
    // load a fixed resources/ path reassembled from .part-* siblings.
    std::string model_path;
    if (data_role) {
        try {
            model_path = data::resolve_role(role).string();
        } catch (const std::exception & ex) {
            throw std::runtime_error(std::string("coder: ") + ex.what());
        }
    } else {
        model_path = resolved_model_path();
        if (!model_chunks::ensure(model_path)) {
            throw std::runtime_error(
                std::string("coder: model file missing and chunks not found: ") + model_path);
        }
    }

    std::uint64_t bytes = 0;
    try { bytes = std::filesystem::file_size(model_path); } catch (...) {}
    auto placement = hardware::pick_placement(role, bytes);

    llama_model_params mp = llama_model_default_params();
    mp.use_mmap = placement.mmap;

    // qwen35 is ~25 GB -> too big for one 16 GB P100. Layer-split it across
    // BOTH cards (fast, ~47 TPS) instead of single-card + UVA overflow (slow,
    // ~4 TPS). This requires both cards clear, so evict the big GPU siblings
    // first (cleanup stays; it is tiny and pinned to card 0).
    static const float kQwen35Split[2] = { 0.45f, 0.55f }; // bias weight to card 1 (card 0 also holds cleanup)
    if (data_role) {
        physics_shutdown_if_loaded();
        chemistry_shutdown_if_loaded();
        vision_shutdown_if_loaded();
        qwen14b_shutdown_if_loaded();
        planner_shutdown_if_loaded();
        mp.n_gpu_layers = 999;
        mp.split_mode   = LLAMA_SPLIT_MODE_LAYER;
        mp.main_gpu     = 0;
        mp.tensor_split = kQwen35Split;
    } else {
        // Legacy single-card path (UVA covers overflow).
        hardware::request_evict(placement.displaced_role);
        mp.n_gpu_layers = placement.n_gpu_layers < 0 ? 999 : placement.n_gpu_layers;
        mp.split_mode   = LLAMA_SPLIT_MODE_NONE;
        mp.main_gpu     = placement.main_gpu;
    }

    std::fprintf(stderr,
        "coder: loading role=\"%s\" path=%s bytes=%.1fg  split=%s  placement: %s\n",
        role.c_str(), model_path.c_str(),
        double(bytes) / double(1ULL << 30),
        data_role ? "LAYER(2-card)" : "NONE(1-card)", placement.reason.c_str());

    llama_model * model;
    {
        bench::LoadScope _bl(role, placement.main_gpu,
                             placement.displaced_role);
        model = llama_model_load_from_file(model_path.c_str(), mp);
        if (!model) {
            _bl.cancel();
            throw std::runtime_error(
                std::string("coder: failed to load GGUF: ") + model_path);
        }
    }
    hardware::note_role_loaded(role, placement.main_gpu);

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

    g_runtime = new Runtime{ model, ctx, placement.main_gpu };
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

const std::string & active_role() { return resolved_role(); }

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
    // Break greedy-loop degeneracy. Without this, a prompt that produces
    // a self-reinforcing prefix (e.g. stylize on a hyphenated part
    // number "ATCA-08-471M-V") locks the argmax path into repeating the
    // same sentence until max_new_tokens fires. Penalty on the last 128
    // tokens applied to logits BEFORE greedy picks the argmax, so
    // output stays deterministic-ish — JSON / LABEL: / REWRITE:
    // format-strict prompts still land on their canonical output. Every
    // qwen14b::generate caller (classify, stylize, render_final, resolve,
    // entities, expertise, disambiguate, components intent, answer,
    // planner) delegates here, so this one line stops the loop
    // pipeline-wide.
    llama_sampler_chain_add(smpl,
        llama_sampler_init_penalties(/*penalty_last_n=*/128,
                                     /*penalty_repeat=*/1.15f,
                                     /*penalty_freq=*/0.0f,
                                     /*penalty_present=*/0.0f));
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

    std::string out;
    out.reserve(static_cast<std::size_t>(max_new_tokens) * 4);

    llama_batch    batch  = llama_batch_get_one(toks.data(), static_cast<int>(toks.size()));
    llama_token    new_id = 0;
    const uint32_t ctx_cap = llama_n_ctx(ctx);
    bool           hit_eog = false;

    bench::GenScope _bg(resolved_role(), rt->main_gpu,
                        static_cast<std::uint64_t>(toks.size()));
    bool prefill_done = false;
    const auto gen_start = std::chrono::steady_clock::now();
    int produced = 0;
    for (; produced < max_new_tokens; ) {
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
        if (!prefill_done) {
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - gen_start).count();
            _bg.set_prefill_ms(static_cast<std::uint64_t>(ms));
            prefill_done = true;
        }
        if (produced % 50 == 0) status::progress_set(resolved_role(), produced, max_new_tokens);
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
    _bg.set_output_tokens(static_cast<std::uint64_t>(produced));

    // Any exit without an end-of-generation token (context full, decode
    // error, max_new_tokens) means the text is an incomplete prefix.
    if (truncated) *truncated = !hit_eog;

    return strip(out);
}

std::string comment_code(std::string_view language_hint,
                         std::string_view source,
                         int max_new_tokens,
                         bool * truncated) {
    // Reuses the currently-loaded coder runtime via generate(): no
    // eviction, no reload. Prompt is engineered to make the model
    // return ONLY the annotated file with executable text preserved
    // verbatim, so the caller can overwrite the original file with the
    // result directly.
    const std::string sys =
        "You are a source code annotator. Your ONLY job is to add"
        " comments to the code you are given.\n"
        "STRICT RULES:\n"
        "  1. Preserve every character of executable code EXACTLY.\n"
        "     Do not rename anything. Do not reformat. Do not reorder.\n"
        "     Do not touch string literals. Do not touch macros.\n"
        "  2. Above each function definition, insert a Doxygen block\n"
        "     comment using \\brief, \\param, \\return, \\note, \\pre,\n"
        "     \\post as appropriate. Multi-line, wrapped at 78 cols.\n"
        "  3. Add // inline comments on non-obvious lines: the why,\n"
        "     not the what. Explain intent, invariants, edge cases,\n"
        "     assumptions, subtle bugs you notice (as \\note in the\n"
        "     header, not a rewrite). Aim for verbose: err on the\n"
        "     side of MORE comments, not fewer.\n"
        "  4. Output the FULL annotated file. No prose before or after,\n"
        "     no ``` fences, no explanation, no diff. Just the file.\n"
        "  5. If the file is already well-commented, still add Doxygen\n"
        "     headers; do not remove existing comments.\n";

    std::string user;
    user.reserve(source.size() + 128);
    user.append("Annotate this ");
    user.append(language_hint);
    user.append(" file per the rules. Return the whole file, "
                "comments-added, nothing else.\n\n");
    user.append(source);
    return generate(sys, user, max_new_tokens, truncated);
}

}
