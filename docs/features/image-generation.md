---
title: Image generation
parent: Features
nav_order: 2
---

# Image generation
{: .no_toc }

Real Chroma1-HD backed image synthesis. The `image_generator` module
shells out to `sd-cli` (built from stable-diffusion.cpp) which runs
the Chroma DiT with a Flux VAE and a T5-XXL text encoder. Output
lands as a PNG under `<project>/.ac9_images/<slug>.png` so the rest
of the pipeline (the image resolver, the editor, LoRA training) can
find it later.

## The `image_gen` tool

The tool router dispatches to `image_gen` when the user asks for a
new image. The shaped prompt strips tool names, forces a save-path
convention, and passes the args through to `image_generator::run`.
Every knob the operator has been asking for is threaded through the
options struct:

- **Reproducible seeds.** `seed = 0` lets sd-cli pick; a non-zero
  value pins the run so the same prompt+seed always produces the
  same PNG.
- **img2img.** Populate `init_img_path` and the generator switches
  to image-conditioned diffusion. `strength = 0.55` is the default
  for identity-preserving "same character, new pose" variants.
- **LoRA references.** Any `LoraRef { name, weight }` in the options
  is appended to the prompt as `<lora:name:weight>` tokens so
  sd-cli's `--lora-model-dir` path picks them up.

## Subject consistency

Chroma1-HD alone drifts between generations. `ac9` layers the
consistency ladder documented in
`modules/1624_image_generator/CONSISTENCY.md` on top of it: seed
locking (Level 0), img2img reference (Level 1), a canonical-character
LoRA trained by an ostris/ai-toolkit wrapper (Level 3). The router
knows which level to reach for based on whether the prompt names an
existing canonical character.
