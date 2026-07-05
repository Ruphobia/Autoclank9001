#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Salience Tuner: per-entry weight, decay rate, and pinning over the
// project / personal memory stores. The store holds raw entries; this
// module adds a side-channel weight model so frequently-relevant
// entries stay hot while stale ones fade. Pinned entries ignore decay
// entirely.
//
// The model is multiplicative-exponential:
//     effective = base * exp(-decay_per_day * days_since_touch)
//     (pinned entries return base unmodified)
//
// State is persisted as a JSON sidecar at <store_dir>/salience.json so
// it is trivial to read, edit, and back up alongside the entries.
namespace salience {

enum class Scope { Project = 0, Personal = 1 };

struct Weight {
    double  base          = 1.0;
    double  decay_per_day = 0.05;
    bool    pinned        = false;
    int64_t touched_unix  = 0;
};

void init();
void shutdown();

void  set_weight(Scope, int64_t id, double base,    std::string_view project_root);
void  set_decay (Scope, int64_t id, double per_day, std::string_view project_root);
void  pin       (Scope, int64_t id,                 std::string_view project_root);
void  unpin     (Scope, int64_t id,                 std::string_view project_root);
// Update touched_unix to `now_unix` (or current time if 0). Boosts
// effective weight by resetting the decay clock.
void  touch     (Scope, int64_t id, int64_t now_unix,
                 std::string_view project_root);

Weight                weight (Scope, int64_t id, std::string_view project_root);
double effective_weight(Scope, int64_t id, int64_t now_unix,
                        std::string_view project_root);

// Sort `ids` by effective_weight at `now_unix`, descending. Entries
// with no recorded weight default to Weight{}.
std::vector<int64_t> rerank(Scope scope, std::vector<int64_t> ids,
                            int64_t now_unix, std::string_view project_root);

}
