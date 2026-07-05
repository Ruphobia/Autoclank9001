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
