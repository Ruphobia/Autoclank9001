# Subject-consistency workflow

This module now supports two levels of subject consistency, matching
the two-week ship plan in `scratchpad/subject_consistency_research.md`:

- **Level 1 — approve-then-vary.** Seed lock + img2img + character sheet.
  Works with Chroma1-HD out of the box. Zero external installation.
- **Level 3 — LoRA training loop.** Trains a per-character
  `<char>.lora.safetensors` against the abliterated Chroma1-HD base via
  `ostris/ai-toolkit`. Requires one-time Python + ai-toolkit setup.

Everything stays uncensored: Chroma1-HD is abliterated by default (no
refusal, no watermark, no C2PA payload — see
`scratchpad/subject_consistency_research.md` §1.14) and LoRAs inherit
the base model's abliteration state. Do not swap Chroma for
`black-forest-labs/FLUX.1-dev`; if a FLUX base is ever needed, use
`aoxo/flux.1dev-abliterated` (see `memory/feedback_abliterated_everywhere.md`).

---

## Storage layout

Per-project, under `<cwd>/.ac9_images/canonical/<char>/`:

```
<char>/
  <char>.png                    approved reference sprite (img2img source)
  <char>.txt                    tag-line prompt suffix appended to every draw
  <char>.seed                   decimal seed reused for every draw
  <char>.lora.safetensors       LoRA weights (Week 2 output; optional)
  training/                     ai-toolkit output dir (Week 2 scratch)
  train.yaml                    ai-toolkit config (rebuilt every run)
  train.log                     last training run's combined stdout+stderr
  sheet/                        optional front/side/back/pose grid
  <extra approved variants>.png other approved images (feed the LoRA trainer)
```

The `image_resolver::canonical_*` helpers read/write this layout — they
are the only source of truth for the paths and slug rules
(`[a-z0-9_]` only; case-insensitive input).

---

## Level 1 — approve-then-vary (Week 1)

Three moving parts:

### 1. Character continuity signals

The tool_router's `image_gen` tool schema now includes an optional
`character_name` arg. The router extracts it whenever the user's
request refers to a recurring character. As a fallback, the dispatch
in `server.cpp` also sniffs a heuristic from the subject phrase
(`the robot`, `our maze robot`, `the same robot`).

### 2. Promote-to-canonical

Once the operator likes a generation, they lock it in:

```
Promote assets/robot_idle.png as karth.
```

The `image_promote` tool routes here. It:

1. Copies the source PNG into
   `<cwd>/.ac9_images/canonical/karth/karth.png`.
2. Rolls a fresh 63-bit seed and writes it to `karth.seed`.
3. Asks the coder LLM for a short tag-line describing the image
   ("blue chibi robot, single red glowing eye-slit, no antenna,
   isometric 3/4 view, matte plastic"). Writes it to `karth.txt`.

If the operator omits the source path, the tool falls back to the most
recent `image` / `gen_path` record in the session store — perfect for
"promote this as karth" said immediately after Chroma returns.

### 3. Reference-aware image_gen

When `image_gen` is dispatched with a `character_name` that has a
canonical, the dispatch loads:

- `canonical_ref(cwd, char)` — used as `-i <path>` (img2img init).
- `canonical_seed(cwd, char)` — used as `-s <seed>` for reproducibility.
- `canonical_prompt_suffix(cwd, char)` — appended to the subject.
- `canonical_lora(cwd, char)` — appended as `<lora:<char>.lora:1.0>`
  (Week 2; ignored if the file doesn't exist yet).

Denoise strength is **0.55**, the identity-preserving sweet spot from
the research report §1.9. Higher (0.7-0.95) frees the model to edit
the character; lower (0.3-0.4) barely changes anything. 0.55 keeps
identity while letting the pose and composition move.

The dispatch emits a `character_locked` SSE layer showing which knobs
were applied, so the AI pane surfaces the routing decision.

---

## Level 3 — LoRA training loop (Week 2)

Once the operator has 5-15 approved images under the canonical dir,
they can lock the character in more strongly:

```
Train a LoRA for karth.
```

The `train_canonical` tool routes here. It:

1. Gathers every valid PNG under the canonical dir (via
   `image_resolver::canonical_training_images`).
2. Delegates to `lora_trainer::train()`, which:
   - Writes an ai-toolkit YAML config into `train.yaml`.
   - Forks `python3 <ai-toolkit>/run.py train.yaml`.
   - Streams combined stdout+stderr through a caller-supplied progress
     callback so the SSE stream can show `step 400/1000 loss=0.148`
     style updates.
3. Locates the newest `.safetensors` under the ai-toolkit output dir
   and lands it as `<char>.lora.safetensors` in the canonical dir.

After success, every future `image_gen` call naming the character
auto-appends the LoRA token — no config change, no restart.

### Defaults

| Knob            | Default | Notes                                    |
|-----------------|---------|------------------------------------------|
| rank            | 16      | Paved Chroma path (research §5).         |
| steps           | 1000    | 500-1000 is the sweet spot for sprites.  |
| learning_rate   | 1e-4    | ai-toolkit default for Chroma.           |
| resolution      | 512     | Bump to 1024 when time allows on P100.   |
| base_model_path | Chroma  | Reuses `SD_CHROMA_MODEL` from env.       |

Wall-clock on 2xP100: ~90-180 min per LoRA. See
`scratchpad/subject_consistency_research.md` §5 for the P100
practicality breakdown.

### One-time operator setup

`lora_trainer::status()` will report `not ready` until this is done.
Neither the ac9 build nor the ac9 runtime installs anything Python.

```bash
# Somewhere the operator picks:
export AC9_AI_TOOLKIT_DIR=/home/jwoods/work/Autoclank9001/scratchpad/ai-toolkit
git clone https://github.com/ostris/ai-toolkit "$AC9_AI_TOOLKIT_DIR"
cd "$AC9_AI_TOOLKIT_DIR"
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

# ac9 spawns python3 as the trainer. When the venv's python3 is the
# right target, either activate the venv before starting ac9 or set:
export AC9_PYTHON_BIN="$AC9_AI_TOOLKIT_DIR/.venv/bin/python3"
```

The manifest entry `lora-trainer` documents this dependency for the
hardware:: role bookkeeping.

---

## Environment overrides

| Env var                            | What it controls                       |
|-----------------------------------|-----------------------------------------|
| `SD_CHROMA_MODEL`                  | Chroma1-HD gguf path (base for both).   |
| `SD_LORA_DIR`                      | Directory sd-cli scans for `<lora:...>`.|
| `AC9_AI_TOOLKIT_DIR`               | ostris/ai-toolkit checkout.             |
| `AC9_PYTHON_BIN`                   | Python interpreter for the trainer.     |
| `AC9_LORA_BASE_MODEL`              | Override the LoRA training base model.  |

---

## Cross-references

- `scratchpad/subject_consistency_research.md` — full landscape report
  and the two-week ship plan this document implements.
- `memory/feedback_abliterated_everywhere.md` — operator policy that
  every model on this path stays uncensored.
- `modules/009_tools/image_resolver/image_resolver.hpp` — `canonical_*`
  helper API (the on-disk source of truth for the layout above).
- `modules/009_tools/lora_trainer/lora_trainer.hpp` — trainer wrapper
  API (options, callback shape, status probe).
- `modules/009_tools/tool_router/tool_router.cpp` — `image_gen`,
  `image_promote`, `train_canonical` tool descriptors.
- `modules/010_interface/server.cpp` — dispatch cases that wire it
  all together.
