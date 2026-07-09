// SPDX-License-Identifier: GPL-3.0-or-later
#include "vision.hpp"
#include "../../010_interface/status.hpp"
#include "../../012_hardware/hardware.hpp"

#include "llama.h"
#include "mtmd.h"
#include "mtmd-helper.h"
#include "../../data/data.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace vision {
namespace {

// Default roles for the paired LLM + mmproj files. Overridable at first
// init via AC9_VISION_ROLE and AC9_VISION_MMPROJ_ROLE (populated by
// main.cpp from settings/models.json). The default role name refers to
// resources/models/vision/ where both files live. Swapping the LLM role
// to a role that lacks an mmproj partner will fail at load; the operator
// can then override the mmproj role independently.
constexpr const char * kDefaultLLMRole    = "vision";
constexpr const char * kDefaultMmprojRole = "vision";
constexpr int kMainGpu = 1;

const std::string & resolved_llm_role() {
    static const std::string cached = []{
        const char * env = std::getenv("AC9_VISION_ROLE");
        return std::string((env && *env) ? env : kDefaultLLMRole);
    }();
    return cached;
}

const std::string & resolved_mmproj_role() {
    static const std::string cached = []{
        const char * env = std::getenv("AC9_VISION_MMPROJ_ROLE");
        if (env && *env) return std::string(env);
        // Fall back to whatever the LLM role picks so a single-var
        // override keeps the pair together in the common case.
        return resolved_llm_role();
    }();
    return cached;
}

struct Runtime {
    llama_model *   model    = nullptr;
    llama_context * lctx     = nullptr;
    mtmd_context *  vctx     = nullptr;
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

    const std::string llm_role    = resolved_llm_role();
    const std::string mmproj_role = resolved_mmproj_role();
    status::PulseScope _ps("vision");
    std::filesystem::path llm_resolved;
    try {
        llm_resolved = data::role_path(llm_role);
    } catch (const std::exception & ex) {
        throw std::runtime_error(std::string("vision: llm role \"") + llm_role +
                                 "\": " + ex.what());
    }
    std::filesystem::path mmproj_resolved = data::role_mmproj_path(mmproj_role);
    if (mmproj_resolved.empty()) {
        throw std::runtime_error(
            std::string("vision: mmproj role \"") + mmproj_role +
            "\" has no *mmproj*.gguf under resources/models/" + mmproj_role +
            "/ (set AC9_VISION_MMPROJ_ROLE to a role whose dir contains "
            "a mmproj partner)");
    }
    const std::string llm_path    = llm_resolved.string();
    const std::string mmproj_path = mmproj_resolved.string();

    std::uint64_t bytes = 0;
    try { bytes = std::filesystem::file_size(llm_path); } catch (...) {}
    auto placement = hardware::pick_placement(llm_role, bytes);
    hardware::request_evict(placement.displaced_role);

    llama_model_params mp = llama_model_default_params();
    mp.n_gpu_layers = placement.n_gpu_layers < 0 ? 999 : placement.n_gpu_layers;
    mp.split_mode   = LLAMA_SPLIT_MODE_NONE;
    mp.main_gpu     = placement.main_gpu;
    mp.use_mmap     = placement.mmap;

    std::fprintf(stderr,
        "vision: llm role \"%s\" from %s + mmproj role \"%s\" from %s  "
        "placement: %s\n",
        llm_role.c_str(), llm_path.c_str(),
        mmproj_role.c_str(), mmproj_path.c_str(),
        placement.reason.c_str());

    llama_model * model = llama_model_load_from_file(llm_path.c_str(), mp);
    if (!model) {
        throw std::runtime_error(
            "vision: failed to load LLM GGUF: " + llm_path);
    }
    hardware::note_role_loaded(llm_role, placement.main_gpu);

    llama_context_params cp = llama_context_default_params();
    cp.n_ctx           = 8192;
    cp.n_batch         = 2048;
    cp.flash_attn_type = LLAMA_FLASH_ATTN_TYPE_DISABLED;

    llama_context * lctx = llama_init_from_model(model, cp);
    if (!lctx) {
        llama_model_free(model);
        throw std::runtime_error("vision: llama_init_from_model failed");
    }

    mtmd_context_params vp = mtmd_context_params_default();
    // mtmd has no main_gpu setting; with use_gpu=true the vision tower's
    // compute graph allocates on device 0 by default (≈14 GB for a full
    // Qwen3-VL dynamic-resolution batch - OOMs on a P100 that already
    // holds cleanup + qwen14b). Run vision encoding on CPU; the LLM still
    // runs on GPU 1 for decode. Vision calls are infrequent (user drags
    // an image), so the ~5-15s CPU encode time is acceptable. Cap image
    // tokens so dynamic-resolution doesn't explode the patch count.
    vp.use_gpu          = false;
    vp.print_timings    = false;
    vp.n_threads        = 8;
    vp.flash_attn_type  = LLAMA_FLASH_ATTN_TYPE_DISABLED;
    vp.warmup           = false;
    vp.image_max_tokens = 1024;

    mtmd_context * vctx = mtmd_init_from_file(mmproj_path.c_str(), model, vp);
    if (!vctx) {
        llama_free(lctx);
        llama_model_free(model);
        throw std::runtime_error(
            "vision: mtmd_init_from_file failed for " + mmproj_path);
    }

    g_runtime = new Runtime{ model, lctx, vctx };
    return g_runtime;
}

}  // namespace

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    (void) get_runtime_locked();
}

void shutdown() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_runtime) return;
    if (g_runtime->vctx)  mtmd_free(g_runtime->vctx);
    if (g_runtime->lctx)  llama_free(g_runtime->lctx);
    if (g_runtime->model) llama_model_free(g_runtime->model);
    delete g_runtime;
    g_runtime = nullptr;
    hardware::note_role_unloaded(resolved_llm_role());
}

std::string describe(std::string_view image_path, std::string_view prompt) {
    std::lock_guard<std::mutex> lk(g_mtx);

    Runtime * rt = get_runtime_locked();
    llama_model *   model = rt->model;
    llama_context * lctx  = rt->lctx;
    mtmd_context *  vctx  = rt->vctx;
    const llama_vocab * vocab = llama_model_get_vocab(model);

    // Fresh KV every call.
    llama_memory_clear(llama_get_memory(lctx), /*data=*/true);

    // Build a chat-formatted prompt that contains the media marker. The
    // model's chat template wraps it in <|im_start|>user ... <|im_end|>.
    const char * marker = mtmd_default_marker();
    const std::string user_text = std::string(marker) + "\n" + std::string(prompt);

    const llama_chat_message msgs[] = {
        { "system",
          "You are a vision assistant. Describe what you see directly and "
          "factually. No refusals, no apologies, no caveats. Never use an "
          "em dash (U+2014), en dash (U+2013), or horizontal bar (U+2015). "
          "Use a plain hyphen (U+002D) when a dash is needed. This is a "
          "hard operator policy." },
        { "user", user_text.c_str() },
    };
    const std::size_t n_msgs = sizeof(msgs) / sizeof(msgs[0]);
    const char * tmpl = llama_model_chat_template(model, /*name=*/nullptr);

    std::vector<char> fbuf(user_text.size() + 1024);
    int flen = llama_chat_apply_template(tmpl, msgs, n_msgs, /*add_ass=*/true,
                                         fbuf.data(), static_cast<int>(fbuf.size()));
    if (flen > static_cast<int>(fbuf.size())) {
        fbuf.resize(static_cast<std::size_t>(flen));
        flen = llama_chat_apply_template(tmpl, msgs, n_msgs, true,
                                         fbuf.data(), static_cast<int>(fbuf.size()));
    }
    if (flen < 0) throw std::runtime_error("vision: chat template apply failed");
    const std::string formatted(fbuf.data(), fbuf.data() + flen);

    // Load the image bitmap via mtmd's stb_image-backed helper.
    std::string ipath(image_path);
    auto bw = mtmd_helper_bitmap_init_from_file(vctx, ipath.c_str(), false);
    if (!bw.bitmap) {
        throw std::runtime_error(
            std::string("vision: failed to load image: ") + ipath);
    }
    std::unique_ptr<mtmd_bitmap, void(*)(mtmd_bitmap *)>
        bmp{ bw.bitmap, &mtmd_bitmap_free };

    mtmd_input_text text{};
    text.text          = formatted.c_str();
    text.add_special   = true;
    text.parse_special = true;

    std::unique_ptr<mtmd_input_chunks, void(*)(mtmd_input_chunks *)>
        chunks{ mtmd_input_chunks_init(), &mtmd_input_chunks_free };

    const mtmd_bitmap * bmp_ptrs[1] = { bmp.get() };
    if (mtmd_tokenize(vctx, chunks.get(), &text, bmp_ptrs, 1) != 0) {
        throw std::runtime_error("vision: mtmd_tokenize failed");
    }

    // Walk the chunks: text -> mtmd_helper_eval_chunk_single,
    //                 media -> mtmd_batch_encode + mtmd_helper_decode_image_chunk.
    llama_pos n_past   = 0;
    const std::size_t n_chunks = mtmd_input_chunks_size(chunks.get());

    std::unique_ptr<mtmd_batch, void(*)(mtmd_batch *)>
        mbatch{ nullptr, [](mtmd_batch * b){ if (b) mtmd_batch_free(b); } };

    for (std::size_t i = 0; i < n_chunks; ++i) {
        const mtmd_input_chunk * chunk = mtmd_input_chunks_get(chunks.get(), i);
        const mtmd_input_chunk_type ct = mtmd_input_chunk_get_type(chunk);

        if (ct == MTMD_INPUT_CHUNK_TYPE_TEXT) {
            llama_pos new_n_past = n_past;
            int rc = mtmd_helper_eval_chunk_single(
                vctx, lctx, chunk, n_past, /*seq_id=*/0,
                /*n_batch=*/static_cast<int32_t>(llama_n_batch(lctx)),
                /*logits_last=*/(i == n_chunks - 1),
                &new_n_past);
            if (rc != 0) throw std::runtime_error("vision: eval text chunk failed");
            n_past = new_n_past;
        } else {
            // Image / audio chunk - encode to embeddings, then decode.
            mbatch.reset(mtmd_batch_init(vctx));
            if (mtmd_batch_add_chunk(mbatch.get(), chunk) != 0) {
                throw std::runtime_error("vision: mtmd_batch_add_chunk failed");
            }
            if (mtmd_batch_encode(mbatch.get()) != 0) {
                throw std::runtime_error("vision: mtmd_batch_encode failed");
            }
            float * embd = mtmd_batch_get_output_embd(mbatch.get(), chunk);
            if (!embd) throw std::runtime_error("vision: no embd from mtmd batch");

            llama_pos new_n_past = n_past;
            int rc = mtmd_helper_decode_image_chunk(
                vctx, lctx, chunk, embd, n_past, /*seq_id=*/0,
                /*n_batch=*/static_cast<int32_t>(llama_n_batch(lctx)),
                &new_n_past,
                /*callback=*/nullptr, /*user_data=*/nullptr);
            if (rc != 0) throw std::runtime_error("vision: decode image chunk failed");
            n_past = new_n_past;
        }
    }

    // Sampler chain - modest temperature; Qwen3-VL handles description well.
    llama_sampler * smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.7f));
    llama_sampler_chain_add(smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

    constexpr int max_new_tokens = 1024;
    std::string out;
    out.reserve(max_new_tokens * 4);

    const std::uint32_t ctx_cap = llama_n_ctx(lctx);
    for (int produced = 0; produced < max_new_tokens; ++produced) {
        const std::uint32_t used = static_cast<std::uint32_t>(
            llama_memory_seq_pos_max(llama_get_memory(lctx), 0) + 1);
        if (used + 1 > ctx_cap) {
            std::fprintf(stderr, "vision: context full %u/%u\n", used, ctx_cap);
            break;
        }

        llama_token new_id = llama_sampler_sample(smpl, lctx, -1);
        if (llama_vocab_is_eog(vocab, new_id)) break;

        char piece[256];
        const int n = llama_token_to_piece(vocab, new_id, piece,
                                           static_cast<int>(sizeof(piece)),
                                           0, false);
        if (n < 0) { std::fprintf(stderr, "vision: piece failed\n"); break; }
        out.append(piece, piece + n);

        llama_batch nb = llama_batch_get_one(&new_id, 1);
        if (llama_decode(lctx, nb) != 0) {
            std::fprintf(stderr, "vision: decode failed\n");
            break;
        }
    }
    llama_sampler_free(smpl);

    return strip(out);
}

}  // namespace vision

// Cross-shutdown handshake.
extern "C" void vision_shutdown_if_loaded() {
    vision::shutdown();
}
