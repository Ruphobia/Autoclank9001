---
title: FAQ
nav_order: 5
---

# FAQ
{: .no_toc }

## What models does ac9 use?

The default fleet is a mix of small abliterated specialists rather
than one big generalist. As of the current build:

- **coder-big** — `Huihui-Qwen3-Coder-30B-A3B-Instruct-abliterated`
  (i1-Q5_K_M). 30B-A3B MoE, roughly 20 GB gguf. Primary code
  producer.
- **planner-30b** — same base family, thinking variant. Constraint
  plans, ticket decomposition, failure analysis for the
  self-healing runner.
- **qwen14b** — Qwen2.5-14B abliterated. Understanding stack:
  cleanup, classify, entity extraction, stylize, render-final,
  task decomposition when the shell routes to it.
- **Chroma1-HD** — the image generation backbone. Chroma DiT + Flux
  VAE + T5-XXL text encoder, driven through `sd-cli`.
- **Qwen3-VL-8B** — vision. Powers the image resolver's
  descriptive matching and any "look at this picture" prompt.

Every model is downloaded on first run by the `data::` bootstrap
subsystem so a fresh clone self-populates `data/`.

## What GPUs does it need?

The bench target is a pair of NVIDIA P100s in a workstation. Any
CUDA card with unified-memory support works, though — the
`hardware::` module enumerates every GPU on the box, picks the
best-fit card per role, and lets CUDA UVA carry any VRAM overflow
to system RAM. The GPU still does 100% of the compute; the CPU
stays idle. That is why "any model fits any card" is a supportable
claim on this project.

Every model runs on a **single** GPU. `LLAMA_SPLIT_MODE_NONE`,
`n_gpu_layers = -1`, and `GGML_CUDA_ENABLE_UNIFIED_MEMORY=1` on
the process env. Splitting a single model across multiple cards
turns out to be measurably slower than single-card plus UVA — a
lesson the operator paid for in benchmark time so you do not have
to.

`hardware::pick_placement` implements LRU rotation across cards so
the previous role stays warm-cached on its card while a different
model is being loaded, which cuts model-swap latency significantly.

## How do I add a new tool?

Add a `tool_router::Tool` entry to the registry in
`modules/009_tools/tool_router/tool_router.cpp`. Four fields:

- `name` — the identifier the router will emit ("my_tool").
- `description` — one line the router shows the classifier LLM so
  it can decide when to pick this tool.
- `args_schema` — a one-line JSON-shape hint, e.g.
  `R"({"target": "<string>", "options": "<string>"})"`.
- `prompt_template` — the shape the tool's downstream LLM call
  wants its input in. Supports `{user_prompt}` plus named args as
  simple `{name}` substitutions.

Then wire the actual dispatch in `server.cpp` under the chat
pipeline — pick the tool name off the `Choice` and call your
handler with `choice.args` and `choice.shaped_system` /
`choice.shaped_user`. See `image_gen` and `image_edit` for the
paved path.
