# Autoclank9001 / ac9 — supervising notes

This project orchestrates 30B-class local LLMs to do coding work. The
supervising agent (Claude) is a multi-trillion-parameter model. **The
little models will hallucinate.** They will invent APIs, drift
namespaces mid-file, forget includes, disagree with headers they wrote
themselves seconds earlier. The scaffolding must assume that.

## THE GOLDEN RULE

**Claude never touches the ac9 work product.**

- Do not Edit, Write, rm, mv, or otherwise modify any file that ac9 or
  its subprocesses produced, under `/home/jwoods/work/Quantiprize/**` or
  any other target project. Not to unblock, not to fix a typo, not
  "just this once."
- The only permitted interventions are:
  1. Fix ac9 itself (this repo).
  2. Wipe the entire target project and reset state so the pipeline can
     run again from a clean slate.
  3. Send a prompt to ac9 that tells its own coder / planner to repair
     its output (via a repair endpoint).
- Every silent hand-fix corrupts the experiment: the outcome becomes
  "ac9 + Claude's rescue edits" instead of "ac9 alone," which is worse
  than useless.

## The self-healing pipeline (design intent)

ac9's ticket runner should treat every layer of failure with progressive
automation and only escalate to the human at the last step.

### Layer 0 — checkpoint on success

After every ticket completes with `ok=true`, snapshot the entire target
project directory into a last-good backup (e.g. `<project>/.backup/`).
Overwrite the previous snapshot atomically. This bounds any single
failure's blast radius to one ticket's worth of change.

### Layer 1 — auto-stop on block

When a ticket ends with `ok=false`, the runner halts immediately — do
NOT roll forward to the next todo. Every downstream ticket that runs
`cmake --build .` as its verification step would otherwise cascade-fail
on the same broken file. Emit `event: run_paused` on the SSE stream so
the client shows a "blocked, awaiting operator" state.

### Layer 2 — auto-decompose

On block, restore the target project from the last-good backup. Feed
the failing ticket's body to the thinking model (`planner-30b` or
whatever `AC9_PLANNER_ROLE` names), asking it to split the ticket into
2-5 smaller sub-tickets that a small coder can knock out one at a time.
Insert the sub-tickets into `.tickets.agile` in place of the failing
ticket. Resume the run from the first sub-ticket.

### Layer 3 — self-repair (one retry)

If a sub-ticket fails, the planner analyzes the failure log and
produces a fix hypothesis. The coder reads the hypothesis + current
code state and emits a targeted patch. Runner applies the patch and
re-runs the sub-ticket. One retry, no infinite loops.

### Layer 4 — hard halt + operator prompt

If Layer 3 still fails, the sub-ticket stays blocked and the runner
stops. At this point Claude may:

- **Read** the code (Read tool on any file under the target project).
- **Prompt** ac9 to have its own AI repair the issue (via a POST to
  something like `/api/tickets/repair` with a targeted correction
  message).
- **Wipe and restart** the entire pipeline if the failure looks
  structural.

Claude may NOT edit the code. See the GOLDEN RULE above.

## Storage layout

- `/home/jwoods/work/.quantiprize-safekeeping/tickets.agile.pristine` —
  the untouched initial ticket set. Sits outside the project so ac9
  can't accidentally overwrite it. Copy back into
  `<project>/.tickets.agile` on a full reset.
- `<project>/.backup/` — most recent last-known-good project snapshot.
  Rebuilt on every successful ticket.

## Model tier notes

- **coder-big** (`AC9_CODER_ROLE=coder-big`,
  Huihui-Qwen3-Coder-30B-A3B-Instruct-abliterated i1-Q5_K_M) —
  30B-A3B MoE, ~20 GB gguf, primary code producer. Hallucination
  failure mode: header/impl divergence, namespace drift, invented
  types. Doxygen prose looks impressive, the actual code contract
  often doesn't hold.
- **planner-30b** — same base family, thinking variant. Used for
  constraint plans, decomposition, failure analysis.
- **qwen14b** — Qwen2.5-14B abliterated, understanding stack
  (cleanup, classify, entities, stylize, render_final, tasks
  decomposition when routed via shell.cpp:2209).

## Hardware discipline (see also `memory/reference_ollama_p100_uva.md`)

- Every model runs on a SINGLE GPU. `LLAMA_SPLIT_MODE_NONE`,
  `n_gpu_layers = -1` (or 999 which llama.cpp treats as all), and
  `GGML_CUDA_ENABLE_UNIFIED_MEMORY=1` on the process env.
- CUDA UVA carries VRAM overflow to system RAM — GPU still does 100%
  of the compute, CPU stays idle. Any model fits any card this way.
- `hardware::pick_placement` implements LRU rotation across cards so
  the previous role stays warm-cached on its card during model swaps.
- Never split a single model across multiple cards. jwoods bench: it
  is meaningfully slower than single-card + UVA.
