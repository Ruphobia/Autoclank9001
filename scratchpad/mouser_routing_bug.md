# Mouser routing miss + stylize/render_final loop - root cause and fix

Prompt that failed:

> how many of these are instock on mouser: ATCA-08-471M-V

Observed behavior:

1. `classify` returned empty (`act=  subtype=`).
2. `stylize::precise` and `stylize::render_final` each looped the same
   sentence - "I'm settling on a single interpretation since the context
   clearly points to Mouser Electronics as the distributor, with
   ATCA-08-471M-V as a part number where M and V are suffixes rather
   than units of measurement." - for roughly 14 iterations before the
   token budget cut them off.
3. The Mouser short-circuit never fired; final answer to the user was a
   Spock-voice rephrase of the question. Real inventory was never
   queried.

There are two independent failures underneath. Both are cheap to fix.

---

## 1. Routing failure

### Where the Mouser lookup lives

- Public API: `namespace components` in
  `/home/jwoods/work/Autoclank9001/modules/009_tools/components/components.hpp`
  (`has_credentials`, `extract_intent`, `search`, `search_with_retry`,
  `format_results`).
- Implementation:
  `/home/jwoods/work/Autoclank9001/modules/009_tools/components/components.cpp`.
  `search()` POSTs to
  `https://api.mouser.com/api/v1/search/keyword?apiKey=<key>` and
  hard-codes `"searchOptions": "InStock"`. Credentials come from
  `settings/credentials.json` → `mouser.api_key` (already populated on
  this machine - I read the file and `mouser.api_key` is set).
- Intent detector: `components::extract_intent()` (line 151 of
  `components.cpp`) prompts the shared 14B (which now delegates to
  `coder::generate`, the qwen35 runtime) for STRICT JSON with fields
  `is_parts_request`, `keyword`, `use_last_results`, etc.

### The routing miss - three overlapping causes

The chat handler `handle_chat` in
`/home/jwoods/work/Autoclank9001/modules/010_interface/server.cpp` runs
the components short-circuit at **lines 4344-4472**:

```cpp
// server.cpp:4351
bool served_by_components = false;
if (!lookup_blocked &&
    (act.act == "question" || act.act == "command") &&        // gate A
    components::has_credentials())                             // gate B
{
    components::Intent it = components::extract_intent(resolved); // uses resolved
    ...
}
```

Cause 1 - **short-circuit placed after the entire understanding stack.**
Between `cleanup` (line ~3630) and this block, `handle_chat` runs
classify (3180), resolve (3354), entities (3366), wikipedia (3383),
dictionary (3429), thesaurus (3436), stylize::precise (3465),
expertise (3477), disambiguate (3486), stylize::render_final (3496).
Every one of these is a qwen35 generation. For an image request the
codebase already solved this problem with an "EARLY image gen / edit
short-circuit" at `server.cpp:3662-3752` that runs right after cleanup
and bypasses the whole stack. There is no equivalent EARLY block for
parts requests, so a Mouser query must survive all of the above before
it even reaches the short-circuit.

Cause 2 - **gate A killed this specific turn.** classify came out empty
(see Failure 2 root cause below - the greedy sampler looped inside the
classifier). `""` is neither `"question"` nor `"command"`, so the
`components::has_credentials()` branch is skipped entirely.

Cause 3 - even if we reached it, `extract_intent` runs the *same*
looping generator. It might have failed to emit a `{}` object at all;
`parse_loose_json` returns an empty `Intent` in that case
(`components.cpp:200-212`), which then falls through with
`is_parts_request=false`.

### Fix shape (Patch A below)

Add an EARLY parts-search short-circuit right after cleanup (mirroring
the EARLY image gen block at line 3662). Gate it on a cheap keyword
sniff - presence of `mouser`, `digikey`, `in stock`/`instock`, or a
manufacturer-part-number-shaped token - so we don't invoke the LLM
intent extractor on unrelated turns. When the sniff hits, invoke
`components::extract_intent(cleaned)` (not `resolved` - resolve_referents
is one of the layers we're skipping), and if `is_parts_request` fires,
run `components::search_with_retry` + `format_results` + emit a synthetic
`final`, then `return false`. Independent of classify's output.

This also fixes the "Layer 1 - auto-stop on block" analogy in
`CLAUDE.md`: catching parts requests before the understanding stack
means a broken classifier or a broken stylizer never blocks a Mouser
query.

---

## 2. Stylize / render_final infinite loop

### Where the sampler lives

Every layer under discussion - cleanup, classify, resolve, entities,
stylize::precise, stylize::render_final, expertise, disambiguate,
`components::extract_intent`, answerer, planner - flows through
`qwen14b::generate()` in
`/home/jwoods/work/Autoclank9001/modules/003_stylize/qwen14b.cpp`, which
is now a pure delegator to `coder::generate()`:

```cpp
// qwen14b.cpp:32
std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int max_new_tokens) {
    return coder::generate(system_prompt, user_msg, max_new_tokens,
                           /*truncated=*/nullptr);
}
```

`coder::generate` lives in
`/home/jwoods/work/Autoclank9001/modules/009_tools/shell/coder.cpp`.
The sampler is built at **lines 274-276**:

```cpp
// coder.cpp:274
llama_sampler * smpl =
    llama_sampler_chain_init(llama_sampler_chain_default_params());
llama_sampler_chain_add(smpl, llama_sampler_init_greedy());
```

**One sampler in the chain: greedy.** No repetition penalty, no
frequency penalty, no presence penalty, no top-k / top-p, no temperature,
no stop-token list beyond the model's EOG token. The generate loop
(lines 291-331) is a straight `sample -> append -> feed back` with an
EOG check at line 317 as the only exit condition besides
`produced == max_new_tokens`.

Greedy sampling on a self-reinforcing prefix is the textbook
loop-degeneracy setup. Once the model emits "I'm settling on a single
interpretation since ..." and the argmax at the end of that sentence
points back at the sentence-opener token, greedy locks into the same
sequence forever. It cannot escape until either (a) the max-token cap
hits or (b) the same argmax path happens to land on EOG. Nothing else in
the chain nudges it off.

Two neighboring modules already reject greedy for this exact reason:
`modules/009_tools/chemistry/chemistry.cpp:161` uses
`llama_sampler_init_temp(0.5f)`, `physics/physics.cpp:180` uses
`0.6f`, `vision/vision.cpp:237` uses `0.7f`. Only coder + planner + the
cleanup module still use pure greedy - and those are exactly the paths
that own the failed turn.

### Fix shape (Patch B below)

Insert an `llama_sampler_init_penalties` node *before* the greedy node
in the chain. Penalizing the last N tokens' logits before greedy picks
the argmax breaks the loop cleanly without introducing randomness - the
next-highest token is still chosen deterministically, so JSON / LABEL /
REWRITE format-strict prompts still land on their canonical output.
Signature from `modules/llama_cpp/include/llama.h:1406`:

```c
llama_sampler_init_penalties(int32_t penalty_last_n,
                             float   penalty_repeat,
                             float   penalty_freq,
                             float   penalty_present);
```

Recommended values: `penalty_last_n=128, penalty_repeat=1.15,
penalty_freq=0.0, penalty_present=0.0`. Repeat-only, moderate strength,
window big enough to see a full looped sentence.

Because every `qwen14b::generate` caller shares this runtime, the fix
lands once and stops the loop for classify, stylize, render_final,
resolve, entities, expertise, disambiguate, components intent, answer,
planner, and web-lookup intent all at once.

---

## Focused patches

### Patch A - routing (server.cpp)

**File:** `/home/jwoods/work/Autoclank9001/modules/010_interface/server.cpp`

**Location:** immediately after the closing `}` of the EARLY image
short-circuit block (line 3752), before the EARLY ticket CRUD short-
circuit at line 3754.

Add:

```cpp
// ==== EARLY parts-search short-circuit ====
// Any prompt that names Mouser / Digi-Key, mentions "in stock", or
// carries a manufacturer-part-number-shaped token routes straight to
// components::extract_intent + Mouser API, bypassing the whole
// understanding stack. The stack burns 30-90 s of qwen35 generation
// to sharpen a query the Mouser handler will only ever keyword-search
// -- worse, its greedy sampler used to loop the stylize + render_final
// layers on the same sentence when the input contained a hyphenated
// part number. Deterministic keyword sniff up top so we do NOT pay
// the extract_intent LLM cost on unrelated turns.
if (components::has_credentials()) {
    auto smells_like_parts = [](const std::string & s) {
        std::string lc; lc.reserve(s.size());
        for (char c : s) lc.push_back(static_cast<char>(
            std::tolower(static_cast<unsigned char>(c))));
        if (lc.find("mouser")   != std::string::npos) return true;
        if (lc.find("digikey")  != std::string::npos) return true;
        if (lc.find("digi-key") != std::string::npos) return true;
        if (lc.find("in stock") != std::string::npos) return true;
        if (lc.find("instock")  != std::string::npos) return true;
        // Manufacturer-part-number-shaped: >=2 hyphens, mixed
        // digits+letters, >=6 chars, no whitespace in the token.
        std::string cur;
        auto looks_like_pn = [](const std::string & t) {
            if (t.size() < 6) return false;
            int hyphens = 0, digits = 0, alpha = 0;
            for (char c : t) {
                if (c == '-') ++hyphens;
                else if (std::isdigit(static_cast<unsigned char>(c))) ++digits;
                else if (std::isalpha(static_cast<unsigned char>(c))) ++alpha;
                else return false;
            }
            return hyphens >= 2 && digits >= 1 && alpha >= 1;
        };
        for (char c : s) {
            if (std::isspace(static_cast<unsigned char>(c)) || c == ':' ||
                c == ',' || c == '?' || c == '.' || c == ';') {
                if (looks_like_pn(cur)) return true;
                cur.clear();
            } else cur.push_back(c);
        }
        return looks_like_pn(cur);
    };
    if (smells_like_parts(cleaned)) {
        components::Intent it = components::extract_intent(cleaned);
        emit("layer", {{"name", "parts_intent_early"},
                       {"content",
                        std::string("is_parts_request=") +
                        (it.is_parts_request ? "true" : "false") +
                        " keyword=\"" + it.keyword + "\" " + it.reasoning}});
        if (it.is_parts_request) {
            std::string used_keyword;
            auto parts = components::search_with_retry(
                it.keyword, used_keyword, /*limit=*/30, /*retries=*/3);
            std::string a = components::format_results(
                parts, used_keyword,
                (it.want_full_list || it.write_to_file)
                  ? static_cast<int>(parts.size()) : 5);
            if (used_keyword != it.keyword) {
                a = std::string("_(broadened search from `") +
                    it.keyword + "` to `" + used_keyword + "`.)_\n\n" + a;
            }
            context::append("components", "response", a, used_keyword);
            emit("layer", {{"name", "mouser"},
                           {"content", std::to_string(parts.size()) +
                                       " parts; keyword=" + used_keyword}});
            json handler_e;
            handler_e["kind"]    = "components_answer";
            handler_e["keyword"] = used_keyword;
            handler_e["answer"]  = a;
            classify::Result fake_act;
            fake_act.act     = "question";
            fake_act.subtype = "parts";
            fake_act.tags    = { "early-parts-route" };
            json fin;
            fin["act"]       = {{"act", fake_act.act},
                                {"subtype", fake_act.subtype},
                                {"tags", fake_act.tags}};
            fin["final"]     = a;
            fin["handler"]   = handler_e;
            fin["expertise"] = "electronic components (Mouser)";
            emit("final", fin);
            hb_stop.store(true);
            if (hb.joinable()) hb.join();
            sink.done();
            return false;
        }
    }
}
```

**Why:** deterministic pre-filter keeps the extract_intent LLM call
off of turns that clearly aren't parts requests; when the sniff hits,
the whole understanding stack is skipped and the Mouser API is invoked
directly. The existing block at line 4344 is left in place unchanged as
a fallback for turns that reach it via a route this early check didn't
cover - same pattern the codebase already uses for image intent.

### Patch B - sampler (coder.cpp)

**File:** `/home/jwoods/work/Autoclank9001/modules/009_tools/shell/coder.cpp`

**Old (lines 274-276):**

```cpp
    llama_sampler * smpl =
        llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());
```

**New:**

```cpp
    llama_sampler * smpl =
        llama_sampler_chain_init(llama_sampler_chain_default_params());
    // Break greedy-loop degeneracy. Without this, a prompt that
    // produces a self-reinforcing prefix (e.g. stylize on a hyphenated
    // part number) locks the argmax path into repeating the same
    // sentence until max_new_tokens fires. Penalty on the last 128
    // tokens applied to logits BEFORE greedy picks the argmax, so
    // output stays deterministic-ish -- JSON / LABEL: / REWRITE:
    // format-strict prompts still land on their canonical output.
    llama_sampler_chain_add(smpl,
        llama_sampler_init_penalties(/*penalty_last_n=*/128,
                                     /*penalty_repeat=*/1.15f,
                                     /*penalty_freq=*/0.0f,
                                     /*penalty_present=*/0.0f));
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());
```

**Why:** greedy alone has no mechanism to escape a self-reinforcing
argmax loop; penalties applied ahead of greedy nudge the sampler off
the repeated tail without introducing randomness. One-line addition,
covers every downstream layer since they all delegate through
`coder::generate`.

---

## How to test the fix

Rebuild ac9 (`cmake --build build -j`), start the server, then in a
fresh chat session send exactly:

> how many of these are instock on mouser: ATCA-08-471M-V

Expected: the SSE stream should emit a `layer` event named
`parts_intent_early` almost immediately after cleanup (no
classify/stylize/render_final layers between them), followed by a
`mouser` layer with `N parts; keyword=ATCA-08-471M-V`, followed by a
`final` event whose `handler.kind == "components_answer"` and whose
`final` text is a markdown block titled `# Mouser results for
\`ATCA-08-471M-V\`` listing the manufacturer part number, price at
qty 1, and an `<N> in stock` line pulled from Mouser's
`AvailabilityInStock` field. Time from send to final should be a few
seconds (one qwen35 pass for `extract_intent` + one HTTPS round-trip
to `api.mouser.com`), not the ~2-3 minutes the original trace showed.
As a secondary check, send a totally non-parts prompt in another
session (`what's the capital of france?`) and confirm the trace still
flows through the classic understanding stack unmodified - the early
sniff should not fire.

---

## Always-on component-health analysis

Additional scope: whenever the Mouser tool touches a part - search,
follow-up, single-PN lookup - it must ALSO run a lifecycle / stocking
/ procurement-risk analysis on every result and surface any anomalies
to the user before answering. Below is the design.

### 1. Mouser API fields that carry the signals

The response object returned by
`POST https://api.mouser.com/api/v1/search/keyword` (documented in
`modules/009_tools/components/MOUSER_API.md`, cross-checked against
Mouser's Search API v2 hub docs) exposes far more per-part detail than
`components.cpp` currently unpacks. `search()` at
`components.cpp:284-303` reads only 9 of them:
`ManufacturerPartNumber`, `Manufacturer`, `Description`,
`MouserPartNumber`, `DataSheetUrl`, `ProductDetailUrl`,
`AvailabilityInStock`, `Category`, and the first `PriceBreaks[0].Price`.
The health analysis needs the rest of the per-part payload:

| Signal                | Mouser JSON field(s)                                    | Notes |
|-----------------------|---------------------------------------------------------|-------|
| Live inventory        | `AvailabilityInStock` (string integer, sometimes empty) | Currently read; needs to be parsed as int, not raw string. |
| On-order / expected   | `AvailabilityOnOrder` (array of `{Quantity, Date}`)     | Populated when Mouser has expected stock arriving. |
| Factory lead time     | `LeadTime` (string, e.g. `"14 Weeks"` or `""`)          | Empty when in stock; populated when back-ordered. |
| Lifecycle             | `LifecycleStatus` (string) and `ProductStatus`          | Values: `Active`, `NRND`, `EOL`, `Obsolete`, `Discontinued`, `Not Recommended for New Design`, blank. |
| Obsolete flag         | `IsObsolete` (boolean-ish string)                       | Redundant with `LifecycleStatus`; check both. |
| NCNR                  | `NonCancelableNonReturnable` (boolean-ish)              | Procurement risk - money locked once ordered. |
| Suggested replacement | `SuggestedReplacement` or `AlternatePackagings[]`       | Only present when supplier flagged a successor. |
| RoHS / compliance     | `ROHSStatus`, `REACHSVHC`, `LeadFreeStatus`             | Strings - `"RoHS Compliant"`, `"Not Compliant"`, `"Contains Lead"`, etc. |
| Conflict minerals     | `CountryOfOrigin`, `ExportControlClassificationNumber`  | ECCN + COO - regulatory-shipment risk. |
| Order rules           | `Min`, `Mult` (both strings)                            | Currently ignored; a `Min` of `2500` on a hobby project is a UX flag. |
| Price break spread    | `PriceBreaks[]` full array                              | Currently only `[0]` read; large qty-1-vs-qty-1k spread is a signal. |
| RA flag / restricted  | `RestrictionMessage`, `ProductAttributes[]`             | Where Mouser stashes RA (Restricted Access), export-controlled, or shipping-restricted notes. |
| Sourcing              | `InfoMessages[]`                                        | Free-text warnings ("Factory lead time only", "See datasheet for MOQ"). |

Some of these are not returned by `search/keyword` but ARE returned by
`search/partnumber` - for exact-PN queries the health pass should
prefer the partnumber endpoint (better field density) and fall back to
keyword. This will require a new helper alongside `search()`.

### 2. Rule set

Ten rules the analysis pass should apply to every `Part` after
population. Each has a stable `code`, a `severity`
(`info` / `warn` / `critical`), and a one-sentence rationale for the
user-visible message.

| Code                     | Trigger                                                                       | Severity | Rationale                                                                 |
|--------------------------|-------------------------------------------------------------------------------|----------|---------------------------------------------------------------------------|
| `OUT_OF_STOCK`           | `parse_int(AvailabilityInStock) == 0`                                          | critical | Zero units on the shelf - order will back-order or fail.                  |
| `LOW_STOCK`              | `0 < InStock < 10`                                                             | warn     | Fewer than 10 units left; risk of exhaustion mid-project.                 |
| `THIN_STOCK`             | `10 <= InStock < 100`                                                          | info     | Only tens available; caution for anything beyond a prototype build.       |
| `LIFECYCLE_RISK`         | `LifecycleStatus` matches `NRND / Not Recommended / Not for new`               | warn     | Manufacturer flagged as not-recommended for new design.                   |
| `EOL`                    | `LifecycleStatus in {EOL, End of Life, Discontinued}` or `IsObsolete==true`    | critical | End-of-life - remaining inventory is what exists; find a replacement.     |
| `LONG_LEAD`              | `parse_weeks(LeadTime) > 26`                                                   | warn     | Factory lead time >6 months; block on procurement, not on design.         |
| `STOCK_GAP`              | `InStock == 0 && sum(AvailabilityOnOrder[].Quantity) == 0 && LeadTime != ""`   | critical | Nothing on shelf, nothing on order, factory lead only - order today.      |
| `NCNR`                   | `NonCancelableNonReturnable == true`                                           | warn     | Non-cancelable / non-returnable - order commits capital irrevocably.      |
| `HIGH_MOQ`               | `parse_int(Min) >= 100 && qty_1_price != ""`                                   | info     | Minimum order quantity >=100; hobby / prototype quantities may not ship.  |
| `RESTRICTED`             | `RestrictionMessage` non-empty OR ECCN in export-controlled list               | warn     | Export / shipping / access restricted - verify destination and end-use.   |
| `NON_ROHS`               | `ROHSStatus` contains `Not Compliant` or `Contains Lead`                       | info     | Not RoHS-compliant - reject for EU / consumer product BOMs.               |
| `PRICE_JUMP`             | `PriceBreaks[last].Price / PriceBreaks[0].Price < 0.15`                        | info     | qty-1 vs. qty-1k break spread >6.7x - probable distributor markup, source direct at volume. |

(That's 12; we can trim `THIN_STOCK` or `NON_ROHS` if the tile budget is
tight - but keeping all 12 is cheap since each is a couple of lines of
C++.)

### 3. Where the analysis lives

The module exists and is not a stub:
`modules/009_tools/components/components.hpp` + `.cpp`. The health pass
belongs INSIDE `components::search()` as a post-fetch transform, right
after the per-Part JSON unpacking loop at `components.cpp:284-303` and
before the topology filter at line 313. Two shape changes:

**components.hpp** - extend `Part` with the new fields the rules read,
and add a `Flag` type + `flags` vector.

```cpp
namespace components {

struct Flag {
    std::string code;      // e.g. "EOL", "LONG_LEAD"
    std::string severity;  // "info" | "warn" | "critical"
    std::string message;   // one-sentence user-visible rationale
    std::string field;     // Mouser field name that triggered it
};

struct Part {
    // ... existing fields ...
    int         in_stock         = -1;   // parsed AvailabilityInStock; -1 = unknown
    int         on_order         = 0;    // sum of AvailabilityOnOrder[].Quantity
    std::string lead_time;               // raw LeadTime string
    std::string lifecycle;               // LifecycleStatus / ProductStatus
    bool        is_obsolete      = false;
    bool        ncnr             = false;
    std::string rohs_status;
    std::string restriction_msg;
    std::string suggested_replacement;
    int         min_qty          = 1;
    int         qty_mult         = 1;
    std::vector<std::pair<int,double>> price_breaks;  // (qty, unit_price)
    std::vector<Flag> flags;             // populated by analyze_health()
};

// Runs the rule set from section 2 against one Part and pushes any
// triggered flags into `p.flags`. Called at the tail of search()
// (and any future search_partnumber()) so every Part downstream has
// its health pre-analyzed.
void analyze_health(Part & p);

// Convenience: highest severity in `flags` - "critical" > "warn" >
// "info" > "" (none). Used by format_results() to sort dangerous
// parts to the top when the user asked for inventory info.
std::string worst_severity(const Part & p);

}
```

**components.cpp** - add `analyze_health` (~60 lines of if/else on the
rule table), then insert one line after the `out.push_back(std::move(pt))`
at line 302:

```cpp
        // (existing unpack, extended to fill the new fields)
        analyze_health(pt);
        out.push_back(std::move(pt));
```

`format_results()` already knows how to render each part; extend it to
prepend a `Flags:` bullet list under each `Part` block when
`p.flags` is non-empty, and to bump critical-flagged parts to the top
of the sort order (before the current price sort).

### 4. Surfacing to the user

The chat handler already emits per-layer SSE frames. Add one new frame
kind, `component_health`, from the early parts short-circuit
(Patch A above) and the fallback parts block at server.cpp:4344. Frame
payload is structured JSON so the front-end can render it as a badge
list, not just prose:

```json
{
  "name": "component_health",
  "content": {
    "keyword": "ATCA-08-471M-V",
    "parts_analyzed": 3,
    "worst_severity": "critical",
    "flags": [
      { "mfg_part_no": "ATCA-08-471M-V",
        "code": "EOL", "severity": "critical",
        "message": "End-of-life - remaining inventory is what exists; find a replacement.",
        "mouser_field": "LifecycleStatus" },
      { "mfg_part_no": "ATCA-08-471M-V",
        "code": "STOCK_GAP", "severity": "critical",
        "message": "Nothing on shelf, nothing on order, factory lead only - order today.",
        "mouser_field": "AvailabilityInStock+AvailabilityOnOrder+LeadTime" }
    ]
  }
}
```

The answerer / coder stack (paths that consume `components_answer`)
should be nudged to LEAD with these flags before answering the user's
literal question. Concrete wiring: the early-parts block builds the
final answer string as

```
> [!warning] Component-health flags for `ATCA-08-471M-V`:
> - **EOL** (Mouser LifecycleStatus): End-of-life - remaining inventory
>   is what exists; find a replacement.
> - **STOCK_GAP**: Nothing on shelf, nothing on order, factory lead
>   only - order today.

<original mouser results here>
```

so the very first thing the user sees is the anomaly, not the count.
For turns that route through the answer handler
(`009_tools/answer.cpp`) instead of the short-circuit, augment its
system prompt with an instruction: "If the user question mentions a
part number and `component_health` flags are present in the layer
trail, open your answer with a one-sentence flag summary before the
numeric answer." Same rule for the coder path when a BOM is being
generated.

### 5. Anomalies with no single Mouser field

Some anomalies can't be read off one column; they need history or
peer-part comparison.

- **Inventory swing** - `AvailabilityInStock` today vs. last-seen
  value in `context::` for this MPN. Actionable now: `context::append`
  already stashes prior component responses; a lookup can diff. Rule
  `INVENTORY_SWING` fires when |Δ/prior| > 0.5 with prior >= 100.
- **Odd-one-out SKU** - same manufacturer, same base MPN (strip
  suffix), different lifecycle status than its family: the -M-V
  variant is EOL but the -M-T variant is Active, so we suggest the
  sibling. Actionable now: needs a second Mouser call
  (`search/keyword` on the stripped root MPN) and a family regroup.
  Rate-limit conscious - only run when the primary lookup returns a
  critical flag.
- **Price out of family** - same MPN family, one variant priced 5x
  the median. Actionable now: same follow-up call as odd-one-out;
  compute median of `PriceBreaks[0].Price` across siblings; rule
  `PRICE_OUTLIER` fires when this / median > 3.0.
- **Distributor-of-one** - Mouser is the only distributor stocking a
  part; a second-source risk that hurts a manufacturable BOM.
  FUTURE: needs Digi-Key / Arrow / Avnet cross-check; deferred until
  those API integrations exist. For now, log the intent (`INFO:
  supplier concentration check deferred`) so the front-end can
  reserve UI real estate for it.
- **Datasheet mismatch** - the description string on Mouser
  disagrees with the header of the linked PDF (wrong voltage, wrong
  package, obsolete cross-reference). FUTURE: needs a PDF fetch
  + OCR/parse pass; expensive and error-prone; defer.
- **Suspicious price drop** - >70% qty-1 price drop vs. prior
  lookup, often a sign of an EOL fire-sale. Actionable now: uses
  same historical context as `INVENTORY_SWING`.

Ship the four "actionable now" rules with the fix. Book the two
"future" rules as follow-ups so they're not forgotten.

---

## Executive summary (updated)

Two ac9 bugs plus one design extension:

1. **Routing miss** - the Mouser short-circuit at
   `server.cpp:4344-4472` runs after the entire understanding stack
   (classify, resolve, entities, stylize, expertise, disambiguate,
   render_final) AND is gated on `act.act == question|command`. The
   pipeline burned that gate twice: first because the whole stack
   fires a qwen35 call per layer on a query that is trivially
   Mouser-routable, and second because `classify` came back empty
   (see #2) so the gate closed. Fix: add an EARLY parts short-circuit
   right after cleanup that keyword-sniffs for `mouser` / `digikey` /
   `in stock` / part-number-shaped tokens, invokes
   `components::extract_intent(cleaned)` when the sniff hits, and
   emits a synthetic `final` bypassing the stack - mirror of the
   existing EARLY image-gen block at server.cpp:3662.

2. **Stylize / render_final loop** - `coder::generate` at
   `coder.cpp:274-276` chains only `llama_sampler_init_greedy()`
   with no repetition penalty, no top-k/p, no stop-on-repeat. Every
   `qwen14b::generate` caller (classify, stylize, render_final,
   resolve, entities, expertise, disambiguate, components intent,
   answer, planner) runs on this greedy chain, so any self-
   reinforcing prefix locks argmax into a sentence loop until
   `max_new_tokens`. Fix: insert
   `llama_sampler_init_penalties(128, 1.15f, 0.0f, 0.0f)` BEFORE
   the greedy node. Deterministic, single-line, covers every affected
   layer at once.

3. **Always-on component-health analysis** - extend `components::Part`
   with the lifecycle / on-order / lead-time / NCNR / restriction /
   RoHS / MOQ / full-price-break fields Mouser already returns; add
   `analyze_health(Part&)` implementing 12 named rules
   (`OUT_OF_STOCK`, `LOW_STOCK`, `THIN_STOCK`, `LIFECYCLE_RISK`,
   `EOL`, `LONG_LEAD`, `STOCK_GAP`, `NCNR`, `HIGH_MOQ`, `RESTRICTED`,
   `NON_ROHS`, `PRICE_JUMP`) each with a stable code + severity +
   rationale + source-field. Populate `Part::flags` at the tail of
   `components::search()` so every downstream consumer inherits the
   analysis. Emit a new SSE `component_health` layer frame from the
   parts short-circuit and prepend a callout block to the answer so
   the user sees anomalies before the count. Four cross-comparison
   rules (`INVENTORY_SWING`, family odd-one-out, `PRICE_OUTLIER`,
   suspicious drop) come along by leaning on `context::` history and
   a follow-up Mouser call on the stripped root MPN. Two anomaly
   classes - distributor-of-one and datasheet-mismatch - are deferred
   (need second-source APIs / PDF parsing) but explicitly booked.

Report file: `/home/jwoods/work/Autoclank9001/scratchpad/mouser_routing_bug.md`.
