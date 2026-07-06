// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// bench:: -- per-ticket benchmark instrumentation. Records every model
// call inside a ticket run so jwoods can answer "what hardware would
// make this suck less."
//
// Recorded per event (jsonl, one object per line):
//   - event="load":     role, gpu, load_ms, displaced_role (or "")
//   - event="generate": role, gpu, prompt_tokens, output_tokens,
//                       prefill_ms, decode_ms, total_ms, decode_tok_s
//
// Storage: <cwd>/.ac9_runs/<ticket_id>.bench.jsonl
// Also stderr'd as a compact one-line summary per ticket at end_ticket().
//
// Zero-cost when a ticket isn't active (begin_ticket() sets the state;
// record_* is a no-op otherwise, so unit tests don't have to bracket
// their model calls).

#include <cstdint>
#include <string>
#include <string_view>

namespace bench {

// Mark the start of a ticket. All record_* calls between begin_ticket
// and end_ticket() are attributed to this ticket. cwd points at the
// project directory (<cwd>/.ac9_runs/<id>.bench.jsonl is where the
// per-event log lands).
void begin_ticket(std::string_view ticket_id, std::string_view cwd);

// Flush the accumulated records to disk, emit a one-line summary to
// stderr, and clear state. Safe to call without a matching
// begin_ticket() (no-op in that case).
void end_ticket();

// Record a model-load event. load_ms is wall-clock from the moment
// llama_model_load_from_file was entered until it returned.
// displaced_role is the role that got evicted from `gpu` to make room
// (empty string if the card was already free).
void record_load(std::string_view role,
                 int              gpu,
                 std::uint64_t    load_ms,
                 std::string_view displaced_role);

// Record a generate() event. prefill_ms is the wall-clock of the FIRST
// llama_decode call (which processes the whole prompt as one batch);
// decode_ms is the wall-clock spent inside the per-token decode loop
// AFTER the first decode returned. total_ms is the whole call.
void record_generate(std::string_view role,
                     int              gpu,
                     std::uint64_t    prompt_tokens,
                     std::uint64_t    output_tokens,
                     std::uint64_t    prefill_ms,
                     std::uint64_t    decode_ms,
                     std::uint64_t    total_ms);

// RAII helper: capture wall-clock from ctor to dtor and (on dtor) call
// record_load with the role + gpu the caller wanted. Use inside
// get_runtime_locked() around llama_model_load_from_file so the load
// timing goes through even on exception.
class LoadScope {
public:
    LoadScope(std::string_view role, int gpu,
              std::string_view displaced_role);
    ~LoadScope();
    // Call when the scope is used but shouldn't emit (e.g. the runtime
    // was already loaded, load_from_file wasn't actually called).
    void cancel();
private:
    struct Impl;
    Impl * impl_{nullptr};
};

// RAII helper: capture prompt token count + wall-clock, record on dtor.
// Caller sets output_tokens and prefill_ms via setters before the
// scope exits.
class GenScope {
public:
    GenScope(std::string_view role, int gpu,
             std::uint64_t prompt_tokens);
    ~GenScope();
    void set_output_tokens(std::uint64_t n);
    void set_prefill_ms(std::uint64_t ms);
    void cancel();
private:
    struct Impl;
    Impl * impl_{nullptr};
};

}  // namespace bench
