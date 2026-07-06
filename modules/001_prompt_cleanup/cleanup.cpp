// SPDX-License-Identifier: GPL-3.0-or-later
#include "cleanup.hpp"

#include "../010_interface/status.hpp"

#include "llama.h"
#include "ggml-backend.h"
#include "../model_chunks.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace prompt_cleanup {
namespace {

constexpr const char * kModelRelPath =
    "resources/models/cleanup/Qwen2.5-1.5B-Instruct-abliterated.Q8_0.gguf";

constexpr const char * kSystemPrompt =
    "You are a strict copy editor. Fix EVERY error in one pass: spelling, grammar, pronoun case, subject-verb agreement, capitalization, punctuation. Output ONLY the corrected text -- no quotes, labels, preface, or explanation. Do not paraphrase or shorten.\nApply ALL rules together:\n1. Capitalize the first word of each sentence and the lone pronoun I; end every sentence with the right punctuation.\n2. Fix every misspelling, including ie/ei errors (recieve->receive, freind->friend, wierd->weird) and dropped or scrambled letters (resturant->restaurant, tommorow->tomorrow, alot->a lot, yestrday->yesterday).\n3. Fix homophones even when the wrong word is itself real English: your->you're (means 'you are'), its->it's, to/too/two, there/their/they're, then/than, no->know, lose/loose, hear/here.\n4. In compound subjects use SUBJECT pronouns and put I last: 'me and him' -> 'He and I'; 'her and me' -> 'She and I'. Never leave him, her, or them as a subject.\n5. After an action verb, change adjectives to adverbs: sit quiet -> sit quietly, drive slow -> drive slowly, speak loud -> speak loudly.\n6. Each, every, one of, either, neither take a SINGULAR verb. Fix the verb's NUMBER; never change tense to fix agreement: 'each of the kids have' -> 'Each of the kids has' (NOT 'had').\n7. In perfect tenses place adverbs between auxiliary and participle: 'had finished already' -> 'had already finished'; 'has gone already' -> 'has already gone'.\n8. Use past perfect (had + participle) when one past action precedes another past anchor (by Friday, by the time, when X arrived).\n9. Split run-on sentences with a period, or join with comma + and/but/so.\n10. NEVER alter, re-case, or 'correct' filenames, paths, code identifiers, shell commands, or any token containing a dot, slash, or underscore: main.cpp stays main.cpp even at the start of a sentence; 001_interface stays 001_interface. Case matters to computers.";

struct Runtime {
    llama_model *   model = nullptr;
    llama_context * ctx   = nullptr;
};

std::once_flag g_backend_once;
std::mutex     g_runtime_mtx;
Runtime *      g_runtime = nullptr;

int pick_gpu_index() {
    const char * env = std::getenv("TOOL_PROMPT_CLEANUP_GPU");
    if (!env || !*env) return 0;
    char * end = nullptr;
    long v = std::strtol(env, &end, 10);
    if (end == env || *end != '\0' || v < 0 || v > 1024) return 0;
    return static_cast<int>(v);
}

void backend_init_once(int /*physical_gpu_index*/) {
    std::call_once(g_backend_once, []{
        // Do NOT setenv CUDA_VISIBLE_DEVICES -- we need BOTH GPUs visible so
        // different layers can pin to different cards via main_gpu.
        // Cleanup uses TOOL_PROMPT_CLEANUP_GPU (default 0); the shell tool's
        // coder model pins itself to GPU 1.
        llama_log_set(
            [](enum ggml_log_level lvl, const char * txt, void *) {
                if (lvl >= GGML_LOG_LEVEL_ERROR) std::fputs(txt, stderr);
            }, nullptr);

        ggml_backend_load_all();
        llama_backend_init();
    });
}

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

// Copy-editing must never re-case identifiers: the model happily turns a
// sentence-initial "main.cpp" into "Main.cpp", the coder then writes
// Main.cpp, and case matters on Linux. Deterministic backstop: for every
// filename-ish or underscore token in the ORIGINAL text, restore its
// exact casing wherever the cleaned text contains it case-insensitively
// (same spelling, different case, same length -- safe in-place).
std::string restore_identifier_casing(std::string_view original,
                                      std::string cleaned) {
    static const std::regex ident_re(
        R"([A-Za-z0-9_./~-]*[A-Za-z0-9_-]\.[A-Za-z0-9]{1,10})"
        R"(|[A-Za-z0-9]+(?:_[A-Za-z0-9]+)+)");
    auto lower = [](std::string s) {
        for (char & c : s) c = static_cast<char>(
            std::tolower(static_cast<unsigned char>(c)));
        return s;
    };
    const std::string orig(original);
    std::string lc_clean = lower(cleaned);
    for (auto it = std::sregex_iterator(orig.begin(), orig.end(), ident_re);
         it != std::sregex_iterator(); ++it) {
        const std::string tok    = it->str();
        const std::string lc_tok = lower(tok);
        std::size_t pos = 0;
        while ((pos = lc_clean.find(lc_tok, pos)) != std::string::npos) {
            cleaned.replace(pos, tok.size(), tok);
            pos += tok.size();
        }
    }
    return cleaned;
}

Runtime * get_runtime_locked() {
    if (g_runtime) return g_runtime;

    backend_init_once(pick_gpu_index());

    llama_model_params mp = llama_model_default_params();
    mp.n_gpu_layers = 999;
    mp.split_mode   = LLAMA_SPLIT_MODE_LAYER;
    mp.main_gpu     = pick_gpu_index();   // TOOL_PROMPT_CLEANUP_GPU, default 0
    mp.use_mmap     = true;

    if (!model_chunks::ensure(kModelRelPath)) {
        throw std::runtime_error(
            std::string("prompt_cleanup: model file missing and chunks not found: ") + kModelRelPath);
    }
    status::loading_set("cleanup");
    llama_model * model = llama_model_load_from_file(kModelRelPath, mp);
    if (!model) {
        throw std::runtime_error(
            std::string("prompt_cleanup: failed to load GGUF: ") + kModelRelPath);
    }

    llama_context_params cp = llama_context_default_params();
    cp.n_ctx           = 4096;
    cp.n_batch         = 4096;
    cp.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_DISABLED;

    llama_context * ctx = llama_init_from_model(model, cp);
    if (!ctx) {
        llama_model_free(model);
        throw std::runtime_error("prompt_cleanup: llama_init_from_model failed");
    }

    status::loading_clear();
    g_runtime = new Runtime{ model, ctx };
    return g_runtime;
}

}

void init() {
    std::lock_guard<std::mutex> lk(g_runtime_mtx);
    (void) get_runtime_locked();
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_runtime_mtx);
    if (!g_runtime) return;
    if (g_runtime->ctx)   llama_free(g_runtime->ctx);
    if (g_runtime->model) llama_model_free(g_runtime->model);
    delete g_runtime;
    g_runtime = nullptr;
}

std::string clean(std::string_view input) {
    std::lock_guard<std::mutex> lk(g_runtime_mtx);

    Runtime * rt = get_runtime_locked();
    llama_model *       model = rt->model;
    llama_context *     ctx   = rt->ctx;
    const llama_vocab * vocab = llama_model_get_vocab(model);

    llama_memory_clear(llama_get_memory(ctx), /*data=*/true);

    const char * tmpl = llama_model_chat_template(model, /*name=*/nullptr);

    const std::string user_str(input);
    const llama_chat_message msgs[] = {
        { "system",    kSystemPrompt },
        { "user",      "lets eat at the new chinese resturant tommorow night" },
        { "assistant", "Let's eat at the new Chinese restaurant tomorrow night." },
        { "user",      "i beleive he recieved my email last wensday" },
        { "assistant", "I believe he received my email last Wednesday." },
        { "user",      "your going to be late for the meeting again" },
        { "assistant", "You're going to be late for the meeting again." },
        { "user",      "me and him are heading downtown after lunch" },
        { "assistant", "He and I are heading downtown after lunch." },
        { "user",      "please drive slow when its raining hard outside" },
        { "assistant", "Please drive slowly when it's raining hard outside." },
        { "user",      "each of the players need new cleats and had finished already with practice" },
        { "assistant", "Each of the players needs new cleats and had already finished with practice." },
        { "user",      user_str.c_str() },
    };
    const size_t n_msgs = sizeof(msgs) / sizeof(msgs[0]);

    size_t fbuf_init = user_str.size() + 1024;
    for (const auto & m : msgs) fbuf_init += std::strlen(m.content) + 32;
    std::vector<char> fbuf(fbuf_init);

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
        throw std::runtime_error("prompt_cleanup: chat template apply failed");
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
        throw std::runtime_error("prompt_cleanup: tokenize sizing failed");
    }
    std::vector<llama_token> toks(static_cast<std::size_t>(needed));
    if (llama_tokenize(
            vocab, prompt.c_str(), static_cast<int>(prompt.size()),
            toks.data(), static_cast<int>(toks.size()),
            is_first, /*parse_special=*/true) < 0) {
        throw std::runtime_error("prompt_cleanup: tokenize failed");
    }

    llama_sampler * smpl =
        llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

    const int input_token_count = static_cast<int>(toks.size());
    const int max_new_tokens = std::max(64, input_token_count * 2 + 32);

    std::string out;
    out.reserve(static_cast<std::size_t>(max_new_tokens) * 4);

    llama_batch    batch  = llama_batch_get_one(toks.data(), input_token_count);
    llama_token    new_id = 0;
    const uint32_t ctx_cap = llama_n_ctx(ctx);

    for (int produced = 0; produced < max_new_tokens; /* in body */) {
        const uint32_t used = static_cast<uint32_t>(
            llama_memory_seq_pos_max(llama_get_memory(ctx), 0) + 1);
        if (used + static_cast<uint32_t>(batch.n_tokens) > ctx_cap) {
            std::fprintf(stderr,
                "prompt_cleanup: context full at %u/%u; truncating output\n",
                used, ctx_cap);
            break;
        }
        const int rc = llama_decode(ctx, batch);
        if (rc != 0) {
            std::fprintf(stderr,
                "prompt_cleanup: llama_decode rc=%d; aborting\n", rc);
            break;
        }
        if (produced % 50 == 0) status::progress_set("cleanup", produced, max_new_tokens);
        status::pulse();
        if (status::generation_cancelled()) break;

        new_id = llama_sampler_sample(smpl, ctx, /*idx=*/-1);
        if (llama_vocab_is_eog(vocab, new_id)) break;

        char piece[256];
        const int n = llama_token_to_piece(
            vocab, new_id, piece, static_cast<int>(sizeof(piece)),
            /*lstrip=*/0, /*special=*/false);
        if (n < 0) {
            std::fprintf(stderr, "prompt_cleanup: token_to_piece failed\n");
            break;
        }
        out.append(piece, piece + n);
        ++produced;

        batch = llama_batch_get_one(&new_id, 1);
    }

    status::progress_clear();
    llama_sampler_free(smpl);

    return restore_identifier_casing(input, strip(out));
}

}
