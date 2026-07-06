// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace status {

// Record a component's lifecycle. `state` is one of "loading", "ready",
// "error". `detail` is a short human-readable hint.
void note(std::string_view component,
          std::string_view state,
          std::string_view detail = {});

// Set the headline displayed in the UI title bar. `ready` toggles the
// overall ready flag (true = all critical components loaded).
void set_overall(std::string_view headline, bool ready);

// Liveness pulses. The model runtimes call pulse() once per decoded
// token; the chat heartbeat thread emits an SSE heartbeat only when the
// count has advanced, so the UI's "thinking" indicator animates only
// while a model is actually producing output.
void          pulse();
std::uint64_t pulse_count();

// Cooperative cancellation for the chat pipeline (the UI stop button).
// begin_turn() marks the start of a pipeline run and returns its epoch;
// request_cancel() cancels the epoch active at that moment; a turn polls
// cancelled(epoch) between stages. generation_cancelled() is the cheap
// per-token check the model decode loops make next to pulse(): true only
// while the CURRENTLY active epoch is cancelled, so a new turn that
// starts after a stop is unaffected.
std::uint64_t begin_turn();
void          request_cancel();
bool          cancelled(std::uint64_t epoch);
bool          generation_cancelled();

// Generation progress: the currently-active generate() call announces
// which role it is running for and how far along it is against its
// max_new_tokens budget. Set once per generate() call; cleared at the
// end. The heartbeat SSE emitter includes these fields in its payload
// so the client can draw a per-stage progress bar.
void progress_set(std::string_view role, int current, int max);
void progress_clear();

// Model loading state. Called at the start of get_runtime_locked() when
// the module is about to call llama_model_load_from_file, and cleared
// after the load completes. Client shows "loading (<name>)" during
// this window instead of "thinking (<name>)".
void loading_set(std::string_view role);
void loading_clear();

// Read-only accessor used by the heartbeat SSE emitter.
struct ProgressSnapshot {
    std::string role;
    int         current{0};
    int         max{0};
    bool        loading{false};
};
ProgressSnapshot progress_snapshot();

// JSON snapshot of current status, served by GET /api/status.
//   {
//     "headline": "...",
//     "ready":    true|false,
//     "components": [
//       {"name":"cleanup", "state":"ready",   "detail":""},
//       {"name":"kb",      "state":"loading", "detail":"downloading 17%"},
//       ...
//     ]
//   }
std::string snapshot_json();

}
