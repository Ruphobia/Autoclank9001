# Subject consistency in ac9's image generation - a full-landscape report

**Date:** 2026-07-07
**Audience:** ac9 operator (lastious@gmail.com)
**Trigger case:** `/home/jwoods/work/ac9_test_001/assets/robot_idle.png` and
`robot_walk.png`. Two "the robot" sprites share only a blue/white/red-eyes
palette - one is a Minecraft-cube head with no antenna, the other an
anime-mecha with a rounded head, red antenna, articulated joints, glowing
visor. They are demonstrably different characters. The pipeline that made
them (see §3) generated each from an independent text prompt with an
independent seed and no shared conditioning at all. This is the raw
text-to-image failure mode.

**Golden constraint (from CLAUDE.md, reiterated by the operator):** every
recommended model and adapter must be abliterated / uncensored / de-safetied.
Refusals, celebrity blocks, watermarks, safety classifiers, and content
filters are all disqualifying. This constraint is threaded through §1, §6,
§7 explicitly.

---

## Executive TL;DR

1. sd-cli (leejet/stable-diffusion.cpp, master-765) ALREADY has flags for
   IP-Adapter's spiritual successors (`--pulid-weights`, `--photo-maker`),
   for Kontext-style multi-reference (`-r/--ref-image`), for ControlNet
   (`--control-net`, `--control-image`), for LoRA (`--lora-model-dir` +
   `<lora:…:1>` prompt tokens), for embeddings (`--embd-dir`), and for
   img2img (`-i` + `--strength`). ac9's `image_generator.cpp` uses
   **none** of them.
2. On **Chroma1-HD** specifically, the good options collapse to: **LoRA
   (works today, inference-only), img2img with `--strength`, seed reuse,
   and prompt-side character-sheet discipline**. PhotoMaker is SDXL-only.
   PuLID in sd-cli is officially FLUX-only (a community ComfyUI port
   adds Chroma). Kontext `-r/--ref-image` needs the FLUX.1-Kontext-dev
   checkpoint - Chroma1-HD doesn't consume ref-image tokens.
3. For **non-face subjects like the robot sprite** the entire
   InsightFace/ArcFace family (PuLID, InstantID, IP-Adapter-FaceID,
   PhotoMaker's class-word system) is useless - those models identify
   *human faces*. The face is where the "identity" lives; a robot has
   none. The right primitives for robots are: LoRA, general IP-Adapter
   (image-embedding CLIP-Vision), reference-only ControlNet, and Kontext
   ref-image.
4. **Recommendation (§7):** Two engineering-weeks in ac9 should ship
   **Level 1 (approve-then-vary via img2img + seed lock + character
   sheet)** in week 1 and **Level 3 (LoRA training on operator-approved
   canonical images)** in week 2. Skip PuLID/PhotoMaker entirely for
   this project - they don't solve robot/prop consistency and PuLID's
   InsightFace dep drags a non-commercial license into the tree.

---

## 1. Taxonomy of approaches

For each: what it is, license, model families, reference count,
inference cost delta, training cost, quality, SOTA year, weakness,
abliteration status.

### 1.1 Text-only prompt engineering (unique names, character sheets, style tokens)

- **What:** Write a rigid character description ("Karth, blue-and-white
  cube-headed robot, red glowing single-slit eye, no antenna,
  chibi-proportions, matte plastic, isometric 3/4 view"), reuse verbatim
  across every generation. Add a project-specific token
  ("`ac9_robot_karth`") the model has never seen - hope for consistent
  hallucination.
- **License / abliteration:** N/A - this is text. Base model matters
  (see §1.14).
- **Reference count:** 0.
- **Inference cost:** free.
- **Training cost:** free.
- **Quality:** poor for anything specific ("blue robot" is stable; the
  particular blue robot is not). Chroma follows detailed prompts better
  than SD1.5 did but still drifts.
- **SOTA:** 2022. Still the baseline.
- **Weakness:** the exact reason the two robots diverged. "Blue robot
  with red eyes" underspecifies everything else.

### 1.2 Textual Inversion (Gal et al., 2022)

- **What:** Train a single ~1 KB text-embedding vector `S*` (a new "word"
  in CLIP/T5 token space) that stands for a concept from 3-5 example
  images. Insert `<S*>` into prompts.
- **License / abliteration:** MIT reference. The embedding lives in
  token space and inherits the base model's refusals; the abliteration
  of the base model carries through.
- **Model families:** SD1.5, SD2.x, SDXL. FLUX/Chroma path exists via
  T5 embeddings but is fragile because Chroma trains a modified T5
  distribution - Textual Inversion for Chroma is not a paved road.
- **Reference count:** 3-5.
- **Inference cost:** ~0.
- **Training cost:** 15-60 min on a modest GPU (2xP100 fine).
- **Quality:** captures *style* and rough identity, not fine detail.
- **SOTA:** 2022.
- **Weakness:** underparameterized - one vector cannot encode "the exact
  robot with these exact joint articulations."

### 1.3 DreamBooth full-model fine-tune (Ruiz et al., 2022)

- **What:** Fine-tune the entire U-Net/DiT on 3-5 subject images with a
  rare identifier token ("`sks robot`"). Adds prior-preservation loss
  to keep the class ("robot") from collapsing.
- **License / abliteration:** paper is Google Research; open reimpls
  (diffusers) are Apache-2.0. Inherits base model's abliteration state
  - so DreamBoothing an abliterated Chroma1-HD produces an abliterated
  custom model.
- **Model families:** SD1.5, SD2.x, SDXL, FLUX. Chroma DreamBooth is
  possible but pointless - you'd be updating 8.9B params for one
  character.
- **Reference count:** 3-5.
- **Inference cost:** unchanged.
- **Training cost:** 5-30 min on an A100, several hours on P100.
- **Quality:** excellent.
- **SOTA:** 2022 - obsoleted by LoRA-DreamBooth for anyone without an
  A100.
- **Weakness:** produces a full checkpoint per character (~9 GB per
  robot for Chroma). Storage-hostile.

### 1.4 LoRA / DreamBooth-LoRA (Hu et al., 2021 / Cloneofsimo, 2022)

- **What:** Same DreamBooth training objective, but the gradient only
  updates a low-rank matrix pair `A·B` inserted into every attention
  layer. Output is 5-30 MB, not 9 GB.
- **License / abliteration:** Apache-2.0/MIT open implementations
  (ostris/ai-toolkit, kohya-ss/sd-scripts). LoRA inherits the base
  model's abliteration - train against `Chroma1-HD` and you get an
  abliterated LoRA. **Do not** train a LoRA against `black-forest-labs/FLUX.1-dev`
  base - that carries FLUX's residual refusal behavior; if you want a
  FLUX-native LoRA path, train against `aoxo/flux.1dev-abliterated`
  (HuggingFace).
- **Model families:** every major model - SD1.5, SDXL, SD3, FLUX,
  **Chroma1-HD supported** by ostris/ai-toolkit.
- **Reference count:** 10-30 captioned images (fewer for narrow
  subjects; 5-10 is enough for a fixed sprite).
- **Inference cost:** ~0 (LoRA applied to weights at load or at runtime;
  sd-cli supports both via `--lora-apply-mode`).
- **Training cost:** ~10-30 min on an A100 for FLUX-family LoRA;
  1-3 hours on 2xP100 (sm_60, no tensor cores - see §5).
- **Quality:** excellent for consistent identity.
- **SOTA:** 2023-2026. Still the workhorse.
- **Weakness:** needs the operator to approve enough seed images to
  train on - 5-10 minimum. Chicken-and-egg problem for a *brand new*
  character (see §6, Level 3).

### 1.5 IP-Adapter (Tencent, 2023)

- **What:** A small trainable module bolted onto cross-attention that
  reads a CLIP-Vision embedding of a reference image and injects
  image-conditioned tokens alongside text tokens. Reference-image
  conditioning at inference time, no per-subject training.
- **License / abliteration:** Apache-2.0. It is a pure conditioning
  module - no refusal circuit of its own; the base model's abliteration
  state governs.
- **Model families:** SD1.5, SDXL - the **base IP-Adapter is not
  released for FLUX or Chroma**. There is community work on FLUX-IP-Adapter
  (XLabs-AI/flux-ip-adapter, InstantX/FLUX.1-dev-IP-Adapter). For
  Chroma the only path is via the community PuLID-Chroma port (§1.11).
- **Reference count:** 1 image, ideally.
- **Inference cost:** +10-20% denoise time.
- **Training cost:** zero (uses pretrained adapter).
- **Quality:** good for style/composition transfer; weaker on fine
  identity than PuLID/LoRA.
- **SOTA:** 2023, improved by IP-Adapter-FaceID (§1.6) for faces.
- **Weakness:** for a specific pose ("walking left") IP-Adapter often
  bakes in the reference pose too. Combine with reference-only
  ControlNet (§1.7) or LoRA (§1.4) for pose freedom.

### 1.6 IP-Adapter-FaceID (Tencent, 2023-24)

- **What:** IP-Adapter variant conditioned on an ArcFace face embedding
  (via InsightFace) instead of raw CLIP-Vision. Much stronger identity
  lock on human faces.
- **License / abliteration:** InsightFace/ArcFace ships under a
  non-commercial license and this contaminates any pipeline using it -
  see [InsightFace #2469](https://github.com/deepinsight/insightface/issues/2469).
  There is **no celebrity filter** per se, but the non-commercial
  license is the real gotcha for redistribution.
- **Model families:** SD1.5 / SDXL. FLUX/Chroma equivalent is PuLID.
- **Reference count:** 1 face crop.
- **Inference cost:** +10-15%.
- **Training cost:** 0.
- **Quality:** excellent for photorealistic human faces.
- **SOTA:** 2024 for SDXL faces.
- **Weakness:** **face-only** - irrelevant to robot sprites, props,
  animals, monsters, mechs. If ac9 ever generates a human character
  portrait this is worth having; for the maze-game robot it is nothing.

### 1.7 ControlNet reference-only / reference-adain (Zhang, Mou 2023)

- **What:** A ControlNet mode that skips the ControlNet weights entirely
  and instead cross-attends the base model's own attention layers to a
  reference image. Three variants: `reference_only`, `reference_adain`
  (AdaIN-style feature normalization), `reference_adain+attn` (both).
- **License / abliteration:** ControlNet is Apache-2.0. Reference-only
  is a preprocessor with no model of its own. Abliteration inherits
  from the base checkpoint.
- **Model families:** SD1.5, SDXL. **Not natively supported on FLUX/Chroma
  in sd-cli.** There are experimental FLUX ports (comfyanonymous
  work-in-progress), but at Chroma there is no first-class support.
- **Reference count:** 1 image.
- **Inference cost:** +30-50%.
- **Training cost:** 0.
- **Quality:** middling - preserves composition and rough features but
  drifts.
- **SOTA:** 2023.
- **Weakness:** on Chroma today: doesn't exist. Not the answer.

### 1.8 Reference / depth / canny / pose ControlNet (Zhang 2023)

- **What:** Structure preservation via an auxiliary signal - a canny
  edge map, depth map, OpenPose skeleton, or M-LSD lines derived from
  a source image. sd-cli's `--canny` flag is a canny-edge
  preprocessor; `--control-net` and `--control-image` wire the model
  in.
- **License / abliteration:** MIT for the ControlNets; base model
  governs abliteration.
- **Model families:** SD1.5, SDXL. Community ControlNet-FLUX exists
  (XLabs-AI, InstantX). No Chroma-specific ControlNet publicly
  distributed as of mid-2026.
- **Reference count:** 1 control image.
- **Inference cost:** +20-30%.
- **Training cost:** 0.
- **Quality:** great for structure (pose, edges, depth); does not
  encode color/texture identity.
- **SOTA:** 2023. Still standard.
- **Weakness:** for our robot: canny preserves outline shape (mouth
  position, antenna shape) but wouldn't stop the color/palette drift
  we already avoided.

### 1.9 img2img with high denoise strength

- **What:** Feed an existing image as init, add noise proportional to
  `--strength` (0.0 = keep the image, 1.0 = fully renoise), and let
  the model re-diffuse. Low strength (0.3-0.5) preserves composition
  and identity; high strength (0.8-0.95) frees the model to
  substantially edit.
- **License / abliteration:** free; abliteration of the base carries
  through.
- **Model families:** all diffusion models. sd-cli supports it via
  `-i` and `--strength` - **ac9's editor
  (`advanced_raster_image_editor_photoshop_gimp_class.cpp`) uses it
  already at strength 0.95** for the "edit" endpoint. Strength 0.55
  is what the generator would need for consistency.
- **Reference count:** 1 image.
- **Inference cost:** identical to text-to-image at the same step
  count.
- **Training cost:** 0.
- **Quality:** excellent for "keep this robot, put it in a new pose"
  at strength 0.5-0.65 - the operator's canonical trick.
- **SOTA:** 2022. Ancient but effective for consistency.
- **Weakness:** at high strength the identity drifts; at low strength
  the pose barely changes. Sweet spot varies per subject.

### 1.10 PhotoMaker (TencentARC, 2024)

- **What:** Stacks ID embeddings from a directory of reference images
  into cross-attention. v2 adds a face-detect preprocessor generating
  `id_embeds.bin`. Uses hard-coded class words: `man, woman, girl, boy`
  - no `robot, dog, spaceship` support.
- **License / abliteration:** Apache-2.0. No celebrity blocker in the
  weights. **Class-word restriction is the real blocker** for
  non-human subjects.
- **Model families:** **SDXL only** in sd-cli (confirmed by
  `docs/photo_maker.md`). No FLUX/Chroma path.
- **Reference count:** 3-5 images.
- **Inference cost:** +5-10%.
- **Quality:** great for humans.
- **SOTA:** 2024 for SDXL humans.
- **Weakness:** SDXL-only, human-only. Wrong tool for both the base
  model and the subject.

### 1.11 PuLID / PuLID-FLUX / PuLID-Chroma (Guo, Yang 2024)

- **What:** "Pure and Lightning ID." Uses ArcFace + EVA-CLIP-L +
  IDFormer to produce a 32×2048 identity embedding, injected via 20
  small cross-attention layers between transformer blocks. State of
  the art for FLUX face identity in 2024-25.
- **License / abliteration:** PuLID weights are Apache-2.0; the
  identity precompute uses InsightFace/ArcFace (non-commercial
  license). No content filter in PuLID itself; no watermark. **Non-commercial
  license contaminates redistribution.**
- **Model families:** FLUX.1-schnell/dev in sd-cli
  (`docs/pulid.md`). PuLID for Chroma exists as ComfyUI ports -
  [PaoloC68/ComfyUI-PuLID-Flux-Chroma](https://github.com/PaoloC68/ComfyUI-PuLID-Flux-Chroma)
  and [jeankassio/ComfyUI-PuLID-Flux-Chroma](https://github.com/jeankassio/ComfyUI-PuLID-Flux-Chroma-Compatibility-)
  - not yet in leejet/sd.cpp.
- **Reference count:** 1 face crop; multi-face fusion supported by
  the extractor.
- **Inference cost:** +10% denoise, ~0 VRAM (per sd.cpp doc).
- **Training cost:** 0.
- **Quality:** SOTA for FLUX-family face ID (2024).
- **SOTA:** 2024.
- **Weakness:** **face-only** - the ArcFace step will fail (or emit
  garbage) on a robot sprite. Useless for the operator's actual
  problem. Also, requires an out-of-process Python precompute step
  (insightface + EVA-CLIP + torchvision).

### 1.12 InstantID (InstantX, 2024)

- **What:** ArcFace embedding + IdentityNet (a ControlNet). Combines
  face identity with landmark control.
- **License / abliteration:** InstantX weights Apache-2.0;
  InsightFace non-commercial contamination applies. Well-documented
  limitation: **maintains reference composition** even against
  prompt (`myByways` blog) - a headshot input begets a headshot
  output.
- **Model families:** SDXL. FLUX/Chroma equivalent is PuLID.
- **Reference count:** 1 face crop.
- **Weakness:** face-only, composition-locked, non-commercial dep.
  Useless for robots.

### 1.13 FLUX Redux / FLUX.1 Kontext / FLUX.2 multi-ref

- **FLUX Redux:** SigLIP dense embeddings aligned with T5 space; the
  official FLUX image-variation adapter. Style + rough subject
  transfer.
- **FLUX.1-Kontext-dev:** *native* image-to-image editing FLUX
  variant. sd-cli exposes this via `-r/--ref-image` (multiple `-r`
  allowed for multi-image ref). This is the **premier reference-image
  path in sd-cli today** - but it needs the Kontext checkpoint, not
  Chroma. `docs/kontext.md` in sd-cli documents the flag.
- **FLUX.2:** as of late 2025 supports up to 10 reference images in
  one generation with strong preservation of identity/product/style
  (Promptus writeup). sd-cli tracks FLUX.2 (`docs/flux2.md` present
  in the local tree).
- **License / abliteration:** FLUX.1-dev is Non-Commercial and has
  residual refusal circuits; the operator's abliteration policy
  requires the **abliterated FLUX.1-dev** variant
  ([aoxo/flux.1dev-abliterated](https://huggingface.co/aoxo/flux.1dev-abliterated)).
  Kontext-dev inherits FLUX.1-dev's license and abliteration state -
  no abliterated Kontext exists as of this writing; would need to be
  produced by re-running the abliteration pass against Kontext-dev.
  FLUX.2 (Klein) has a permissive Apache-2.0 open weight release;
  refusal behavior not yet independently audited.
- **Reference count:** 1-10.
- **SOTA:** end-2025 through 2026.
- **Weakness:** *for Chroma users:* wrong base model. Kontext ≠ Chroma;
  Chroma1-HD does not consume `-r` reference tokens.

### 1.14 Chroma1-HD (lodestones, Aug 2025) - the base model ac9 uses

- **What:** 8.9B FLUX.1-schnell-derived, de-distilled, retrained on
  5M curated images. Apache-2.0.
- **Abliteration:** the model card explicitly states "not aligned
  with a specific safety filter" and "has the potential to generate
  content that may be considered harmful, explicit, or offensive."
  No refusal, no watermark, no Stable-Signature payload. **This is
  the exemplar of an abliterated diffusion model in 2026 - it was
  born uncensored.** Operator can keep using it.
- **Consistency-relevant features:** LoRA works. img2img works.
  **PhotoMaker, PuLID, ControlNet-reference-only, Kontext ref-image -
  none work natively out of the box on Chroma in sd-cli today.**

### 1.15 IC-Light / OmniGen / OmniGen2 (VectorSpaceLab, 2024-25)

- **What:** Unified any-to-image models. OmniGen2 (Jun 2025) has a
  single "in-context" head that accepts any mix of text + reference
  images and produces subject-driven output. Includes reflective
  refinement.
- **License:** Apache-2.0, open weights.
- **Abliteration:** unfiltered by design (research release). No
  known refusal circuit or watermark.
- **Model families:** its own architecture - not swappable with
  Chroma. Would replace, not augment.
- **Reference count:** 1-N.
- **SOTA:** mid-2025.
- **Weakness:** not integrated into sd-cli; would require a separate
  runner (PyTorch, needs 40+ GB VRAM in fp16, brutal on P100).
  Interesting long-term option, not tractable in a two-week ac9 sprint.

### 1.16 Consistent-character prompt-side pipelines
        (StoryDiffusion, ConsiStory, CharacterFactory, ReMix, StorySync)

- **What:** *Training-free* techniques that share attention/features
  across the images in a batch to enforce consistency ("all N images
  are the same subject").
- **License:** MIT/Apache-2.0 in reference implementations.
- **Model families:** SD1.5 and SDXL are the reference targets;
  ConsiStory-for-FLUX exists in research code.
- **Reference count:** 0 (batch-internal consistency).
- **Inference cost:** ConsiStory ~2x baseline; StoryDiffusion +50%.
- **Quality:** good for a fixed batch; poor once you close the batch
  and later want to add "one more" image in the series.
- **SOTA:** 2024.
- **Weakness:** batch-lock. Doesn't help when you approve one image
  today and want to add a jumping variant tomorrow.

### 1.17 Multi-view / 3D-aware diffusion (MVDream, Zero123++, CAT3D, TripoSR, Zero-P-to-3)

- **What:** One reference image → many camera views, or a 3D asset.
  MVDream trains 4-view consistency into the diffusion loop; Zero123++
  goes single-image to consistent NxM views; TripoSR extracts a 3D
  mesh in a second. CAT3D unifies to full scenes.
- **License:** varies - TripoSR MIT, MVDream MIT, Zero123++ CC-BY-NC-4.0
  (non-commercial contamination on Stability's variants), CAT3D
  proprietary.
- **Abliteration:** neither refusals nor watermarks are typical in
  these research releases, but Zero123++'s CC-BY-NC license is a
  redistribution problem.
- **Model families:** their own.
- **Weakness:** great for turntable/walkaround; wrong shape for
  "walking left" vs "walking right" 2D sprite frames. Real value: if
  the operator wanted the robot as a rigged 3D mesh, TripoSR is the
  cheapest path from one approved sprite.

### 1.18 Closed-source SOTA (for context only)

- **Gemini 2.5 Flash Image ("Nano Banana"):** multi-image-in, text +
  image control, character preservation. **Embeds SynthID
  watermark** - disqualified.
- **GPT-4o image gen:** identity preservation via native
  multimodality. **Embeds C2PA metadata + invisible watermark
  (OpenAI).**
- **Ideogram 3 / Ideogram Character:** identity + face reference API.
  Watermark on free tier.
- **Midjourney character reference (`--cref`):** character consistency
  from URL. No local option.
- **Verdict for ac9:** all closed, all watermarked, none operable
  locally. Ignore for engineering.

---

## 2. What the operator actually wants - workflow archetypes

Rank each against the maze-game robot use case
("generate a base sprite the user likes; make N variants (walking,
jumping, facing 4 directions) that are visibly the same robot;
iterate later without re-approving the base").

### 2.A Approve-then-vary

- **The one:** user picks `robot_idle.png` as canonical; system
  generates `robot_walk.png`, `robot_jump.png`, `robot_left.png`,
  `robot_right.png` as controlled variants.
- **Best fit techniques:** img2img at strength 0.5-0.65 (§1.9),
  seed reuse (§2.persistence), Kontext `-r` if base were FLUX (§1.13),
  IP-Adapter if base supported it (§1.5).
- **On Chroma today:** img2img + seed lock + prompt tokens is the
  *only* zero-code path. This is the day-1 win in §6 Level 1.
- **Verdict:** ★★★★★ correct primary workflow for the operator's
  actual complaint. Everything else is a refinement.

### 2.B Iterative refinement ("make her hair shorter")

- The `advanced_raster_image_editor_photoshop_gimp_class.cpp` edit
  endpoint is already this - img2img strength 0.95 with edit prompt.
- Weakness of current impl: strength 0.95 loses identity. For
  consistency-preserving edits, drop to 0.5-0.7.
- **Verdict:** ★★★★ already 60% built; needs a "preserve identity"
  strength knob.

### 2.C Character sheet

- System produces a canonical front/side/back/3-quarter/pose grid
  once, then every downstream variant is generated with the grid as
  a fixed ref image.
- On FLUX-Kontext: trivial (`-r sheet.png`).
- On Chroma: no ref-image; must go via LoRA trained on the sheet.
- **Verdict:** ★★★★ excellent for the maze game specifically (its
  sprite set is small and finite). Character sheet → LoRA → automatic
  variant generation.

### 2.D Persistent identity token (LoRA per project)

- Train `.ac9_images/canonical/karth.lora` once. Every future
  `image_gen` for character `karth` appends `<lora:karth:1.0>`.
- The gold standard for long-lived projects.
- **Verdict:** ★★★★★ this is the ac9 endgame. §6 Level 3.

---

## 3. What ac9 has right now

Reading `modules/1624_image_generator/image_generator.cpp` and
`modules/1620_advanced_raster_image_editor_photoshop_gimp_class/*.cpp`:

**Current sd-cli invocation (generator):**
```
sd-cli
  --diffusion-model Chroma1-HD-Q8_0.gguf
  --vae             ae.safetensors
  --t5xxl           t5-v1_1-xxl-encoder-Q8_0.gguf
  -p <prompt>
  --cfg-scale 4.0 --sampling-method euler --steps 20
  -H 1024 -W 1024
  --model-args chroma_use_dit_mask=false
  --clip-on-cpu --vae-tiling
  -o <out.png>
```

**Editor** adds `-i <input>` and `--strength 0.95`. That is the entire
subject-consistency surface today: **zero seed lock, zero LoRA, zero
reference conditioning, zero character sheet, zero identity token**.
Every prompt is an independent draw from Chroma's prior.

**Flags sd-cli exposes but ac9 doesn't use (verified by
`sd-cli --help`, sd.cpp master-765):**

| sd-cli flag | Purpose | Chroma-supported? |
|---|---|---|
| `-s / --seed <int>` | Reproducible random seed | Yes |
| `-i / --init-img` | img2img init image | Yes (editor uses; generator doesn't) |
| `--strength <f>` | img2img denoise strength | Yes |
| `--lora-model-dir` + `<lora:name:w>` in prompt | LoRA | **Yes - Chroma LoRA works** |
| `--embd-dir` | Textual Inversion embeddings | Nominally; Chroma fragile |
| `-r / --ref-image` | Kontext multi-reference | **No** - needs FLUX.1-Kontext-dev |
| `--photo-maker`, `--pm-id-images-dir`, `--pm-id-embed-path` | PhotoMaker | **No** - SDXL-only |
| `--pulid-weights`, `--pulid-id-embedding`, `--pulid-id-weight` | PuLID face ID | **No in sd-cli** - FLUX-only; Chroma path only in ComfyUI ports |
| `--control-net`, `--control-image`, `--control-strength` | ControlNet | Community FLUX ControlNets exist; no Chroma release |
| `--canny` | Canny preprocessor | Yes for control-image pipeline |
| `--clip_vision` | CLIP-Vision encoder | For IP-Adapter etc.; not wired in ac9 |
| `--params-backend` / `--split-mode` / `--max-vram` / `--auto-fit` | Placement | Not used; ac9 relies on `evict_all_llms()` |

**Nothing needs to be added to sd-cli today** to ship Level 1 and
Level 3. The binary already speaks LoRA, seed, and img2img - ac9 just
doesn't call them.

**What would need to be added to sd-cli for Level 2 (§6):**
- To use Kontext `-r/--ref-image`: swap base model to `FLUX.1-Kontext-dev`
  (or add it alongside Chroma1-HD). No sd-cli code change - just add
  the checkpoint and route `edit()`/`generate()` to it when a canonical
  ref exists.
- To use IP-Adapter on Chroma: **would require an sd-cli rebuild
  against a Chroma-IP-Adapter fork** - there is no first-party support.
  Not tractable in two weeks; not recommended.
- To use PuLID on Chroma: same - community ports live in ComfyUI, not
  leejet/sd.cpp. Not tractable, and PuLID is face-only so useless for
  robots regardless.

---

## 4. Chroma-specific reality

Chroma1-HD is a **de-distilled FLUX.1-schnell derivative** (12B → 8.9B
by replacing an oversized timestep encoder with a smaller FFN), trained
on 5M curated images including artistic/photographic/niche styles. It
is **the abliterated FLUX for image generation** - Apache-2.0, no
safety alignment, no watermark, no refusal circuit. Full stop.

But its reference-image ecosystem is thin:

- **LoRA:** first-class. ostris/ai-toolkit lists `lodestones/Chroma1-Base`
  as a supported target. sd-cli's `--lora-model-dir` +
  `<lora:name:1>` works out of the box.
- **PuLID:** **no native support in leejet/sd.cpp** (docs/pulid.md
  explicitly lists FLUX-schnell/dev only). Community ports for ComfyUI
  exist. Would need a sd.cpp PR to land Chroma-PuLID; not shipped as
  of master-767 (Jul 2026).
- **PhotoMaker:** SDXL-only. Not coming to Chroma; the class-word
  system is architecture-specific.
- **Kontext `-r` ref-image:** works only with the FLUX.1-Kontext-dev
  checkpoint. Chroma1-HD does not consume ref-image tokens - passing
  `-r` to it is either an error or silently ignored.
- **ControlNet:** no lodestones-shipped ControlNet for Chroma.
  Community FLUX ControlNets (XLabs, InstantX) partially work
  through the shared FLUX backbone but not with Chroma's re-tuned
  weights.
- **FLUX Redux:** designed for FLUX.1-dev; not for Chroma.

**Bottom line:** on Chroma the operator's practical toolbox is
`LoRA + img2img + seed + prompt discipline`. For richer
reference-image conditioning, the base model has to change (to
Kontext-dev-abliterated or FLUX.2 Klein), or a Chroma-native
IP-Adapter/PuLID has to be added to sd-cli by a future contributor.

---

## 5. P100 (sm_60) practicality

Operator has 2× Tesla P100 16 GB (Pascal, sm_60, no tensor cores,
HBM2, 720 GB/s, ~9.3 TFLOPS FP32 / ~18.7 TFLOPS FP16).

| Technique | Feasible on 2xP100? | Wall-clock estimate |
|---|---|---|
| Text-only prompt engineering | trivial | 0 s |
| Textual Inversion training (SDXL) | yes | 20-60 min / concept |
| Textual Inversion training (Chroma) | fragile - not paved | n/a |
| LoRA training (SDXL) | yes | 30-60 min for 500 steps |
| **LoRA training (Chroma1-HD, rank 16, 10 imgs, 1000 steps)** | **yes** - needs Q8 base + fp16 optimizer state on card 1, LoRA weights on card 2 with UVA | **~90-180 min** (P100 has no tensor cores; roughly 2-4x slower than A100 sd-scripts baseline of 20-40 min) |
| LoRA training (FLUX.1-dev abliterated, rank 16, 10 imgs) | yes with QLoRA (nf4/int4 base) | **~2-4 h** |
| DreamBooth full fine-tune | technically yes with heavy CPU offload; not practical | 12+ h and painful |
| **Chroma inference at 1024x1024 x 20 steps** (baseline) | current behavior | ~7 min per generation (from `image_generator.cpp` comment) |
| Chroma inference + LoRA | +5-10% | ~7-8 min |
| Chroma inference + img2img | equal to baseline | ~7 min |
| Kontext-dev inference + `-r` (if migrated) | +20% | ~8-9 min |
| PuLID-FLUX inference (if operator ever needs face) | +10% | ~8 min plus one-shot precompute (~30s on GPU) |
| OmniGen2 (if ever integrated) | ~40 GB in fp16 - needs UVA overflow to RAM; slow | not recommended |

**Verdict:** P100 handles everything ac9 needs *except* full DreamBooth
and the multimodal giants. LoRA training and img2img are firmly in
range. The `hardware::pick_placement` LRU and CUDA UVA path (CLAUDE.md)
extend the reach for the LoRA optimizer state.

---

## 6. Concrete ac9 integration recipes

### Level 1 - day-1 wins, no new modules

Everything sd-cli already exposes; ac9 just calls it. Ship these first;
they solve 60-70% of the operator's complaint alone.

**L1.1 Seed lock across an image series.**
Add a `seed` field to `ImageIntent` (or whatever the tool_router
passes). Default -1 (random). When the operator says "generate
`robot_walk`" *and* `robot_idle` was generated in this session,
propagate `robot_idle`'s seed to the follow-up. sd-cli takes
`-s <int>`. Same seed + closely-related prompt is stable-diffusion's
oldest identity trick.

**L1.2 img2img on the last canonical image.**
When a series is in progress, route `image_generator::generate()`
through `-i <last_canonical.png>` `--strength 0.55` `-s <same_seed>`.
This is exactly what `advanced_raster_image_editor_photoshop_gimp_class::edit`
does today at strength 0.95, retuned for identity-preservation
instead of complete-replacement.

**L1.3 Prompt-side character sheet.**
Add an `.ac9_images/canonical/<project>/character_sheet.txt` file
holding a locked description. Every `image_gen` prompt gets it
prepended. Example:
```
[karth v1] blue-and-white cube-head robot, single red glowing
horizontal eye-slit, no antenna, chibi proportions, matte plastic
body, isometric 3/4 view, plain neutral background,
<prompt>
```
This is the poor man's Textual Inversion. Chroma follows detailed
descriptors well; the drift the operator sees is because "blue robot
with red eyes" was 90% of what was locked.

**L1.4 First approved image auto-promoted to canonical.**
After a `run_ok` on `image_gen`, prompt the operator in the AI pane:
"promote as canonical for `<project>/<character>`?" If yes, symlink
to `.ac9_images/canonical/<character>.png`. Now L1.2 always has a
target.

**Estimated engineering:** 2-3 days. Zero sd-cli work. Two new fields
on `ImageIntent`, one new endpoint on the server, one dir under the
project.

**Payoff:** the two robots would not have diverged. `robot_walk.png`
would have been generated with `-i robot_idle.png -s <same> --strength 0.55`
and would visibly be the same character.

### Level 2 - small module addition (reference conditioning)

Two options. Do the first; skip the second.

**L2.1 (do this): FLUX.1-Kontext-dev abliterated as a second base model,
     wired to `-r/--ref-image`.**
- Download Kontext-dev (see `docs/kontext.md`). Run the aoxo
  abliteration recipe against it (~few hours on P100) so the
  operator's uncensored-everything policy holds. Or, if that recipe
  is not yet published for Kontext specifically, take the general
  abliteration script from
  [aoxo/flux.1dev-abliterated](https://huggingface.co/aoxo/flux.1dev-abliterated)
  and re-run it against Kontext-dev.
- Add `SD_KONTEXT_MODEL` env var alongside `SD_CHROMA_MODEL` in
  `image_generator.cpp`.
- New generator branch: when a canonical reference exists, invoke
  sd-cli with `--diffusion-model <kontext-abliterated.gguf>` and
  `-r <canonical.png>` instead of Chroma. Same T5, same VAE.
- Multi-ref (up to 4 today; up to 10 with FLUX.2) via multiple `-r`.
  Perfect for a 4-direction sprite set: pass all 4 previously-approved
  frames as references when generating the 5th.
- **Watermark check:** Kontext-dev has no known Stable-Signature
  watermark; verify by running the abliterated build through the
  standard steg detectors.
- **Est effort:** 3-4 days once Kontext-abliterated exists.

**L2.2 (skip this): IP-Adapter / PuLID on Chroma.**
No first-party support in leejet/sd.cpp. Would require porting the
ComfyUI Chroma-PuLID community work into a C++/ggml implementation.
Weeks of work, mostly benefiting *face* subjects - a bad ROI for the
operator's actual robot/prop use case. **PuLID is also
InsightFace-licensed (non-commercial contamination).** Do not build
this until the operator's project pivots to human portraits.

### Level 3 - LoRA training loop (the endgame)

After the operator approves a canonical image (or, better, 5-10
approved variants - solved by looping L1 first), auto-kick a LoRA
fine-tune.

**L3.1 Training script.**
- Install `ostris/ai-toolkit` next to sd-cli. It supports Chroma1-HD
  training as a first-class target (base `lodestones/Chroma1-Base`
  or `lodestones/Chroma1-HD`). MIT-licensed, works on Pascal.
- Adapt `config/examples/train_lora_flux_schnell_24gb.yaml` to
  Chroma; rank 16, learning rate 1e-4, 1000 steps, 512-1024 mixed
  res. Kohya-ss also works but ai-toolkit is the paved Chroma path.
- **Abliteration:** train against the abliterated Chroma1-HD base
  (which is the only Chroma there is - it's uncensored by default).
  Confirm no distillation from FLUX.1-dev refusal-heavy weights
  sneaks in; ostris configs default to loading directly from
  Chroma1-HD.gguf which is safe.
- **Face-detection preprocessing:** DISABLE any InsightFace/ArcFace
  step in the ai-toolkit config. For robot/prop training the standard
  BLIP-2 or manual captioning is fine; face-detect would fail on a
  robot anyway.

**L3.2 Storage layout.**
```
.ac9_images/canonical/
  <character>/
    approved/          # 5-15 user-approved PNGs at 1024
      *.png
    captions/          # BLIP-2 or manual "a blue chibi robot ..." text
      *.txt
    <character>.lora.safetensors   # ~30 MB trained output
    train.yaml         # ai-toolkit config
    train.log          # last training log
```

**L3.3 Runner integration.**
- New ac9 module `modules/1626_lora_trainer/` (name TBD) with a
  `train(character_name)` entrypoint. Same eviction pattern as
  `image_generator.cpp::evict_all_llms()` - the ai-toolkit trainer
  wants both GPUs so nothing else can be resident.
- Runs in the background (a `.tickets.agile` variant, or just a
  detached child). Emit `event: lora_train_progress` on the SSE
  stream so the AI pane can show step count.
- On completion, `image_gen`'s tool_router template appends
  `<lora:<character>:1.0>` to every prompt that names the character.
  The router's existing prompt-templating layer (`tool_router.cpp`)
  is the place - add a per-project character registry
  (`.ac9_images/canonical/<character>/manifest.json`) and a prompt
  post-processor that scans for character names and adds the LoRA
  tag.

**L3.4 Wall-clock reality on 2xP100.**
- 10 images × 1000 steps × rank 16 → **90-180 min**. Well within the
  operator's tolerance if kicked off asynchronously.
- Inference cost after training: unchanged (LoRA loaded at load or
  at_runtime by sd-cli; docs/lora.md).

**Est effort:** 4-6 days. The heavy lifting is packaging ai-toolkit
into ac9's build tree and getting the ticket runner to treat a
training run like a first-class layer event.

---

## 7. What to build first (two engineering-weeks recommendation)

**Week 1: Level 1 (approve-then-vary with seed + img2img + character
sheet), all four parts.** This alone would have prevented the
robot_idle/robot_walk divergence, needs zero sd-cli changes, adds two
fields to `ImageIntent` and one on-disk directory, and ships end-to-end
in 2-3 focused days. Do it because it's the biggest ratio of
consistency-gained to code-added anywhere on this map. Add the
"promote to canonical" affordance to the AI pane and the tool_router
prompt-templating that auto-prepends the character sheet.

**Week 2: Level 3 (LoRA training loop against abliterated Chroma1-HD).**
Wire ostris/ai-toolkit into ac9, add `modules/1626_lora_trainer`, ship
the `.ac9_images/canonical/<character>/` storage layout, and teach the
tool_router to auto-append `<lora:<character>:1.0>` for characters that
have a trained LoRA. Chroma1-HD is already abliterated (Apache-2.0, no
refusal, no watermark, per lodestones' model card), so LoRAs inherit
that state cleanly. No InsightFace, no ArcFace, no PhotoMaker
class-words - everything stays uncensored and works on non-face
subjects like the robot. Skip Level 2 (Kontext ref-image) unless the
operator explicitly wants multi-reference in a future sprint;
Level 3's LoRAs are a strictly better identity anchor for long-lived
projects and don't require adopting a second base model. Skip PuLID,
PhotoMaker, IP-Adapter-FaceID entirely - they solve human-face
consistency, not maze-robot consistency, and PuLID drags a
non-commercial InsightFace license into the tree. If a future project
needs face identity, revisit; today it is off-scope.

---

## Sources

- [lodestones/Chroma1-HD (HuggingFace)](https://huggingface.co/lodestones/Chroma1-HD)
- [lodestones/Chroma1-Base (HuggingFace)](https://huggingface.co/lodestones/Chroma1-Base)
- [leejet/stable-diffusion.cpp (GitHub)](https://github.com/leejet/stable-diffusion.cpp)
- [sd.cpp docs/photo_maker.md](https://github.com/leejet/stable-diffusion.cpp/blob/master/docs/photo_maker.md)
- [sd.cpp docs/pulid.md](https://github.com/leejet/stable-diffusion.cpp/blob/master/docs/pulid.md)
- [sd.cpp docs/kontext.md](https://github.com/leejet/stable-diffusion.cpp/blob/master/docs/kontext.md)
- [sd.cpp docs/chroma.md](https://github.com/leejet/stable-diffusion.cpp/blob/master/docs/chroma.md)
- [sd.cpp docs/lora.md](https://github.com/leejet/stable-diffusion.cpp/blob/master/docs/lora.md)
- [ostris/ai-toolkit (Chroma LoRA trainer)](https://github.com/ostris/ai-toolkit)
- [ToTheBeginning/PuLID (upstream)](https://github.com/ToTheBeginning/PuLID)
- [guozinan/PuLID weights](https://huggingface.co/guozinan/PuLID)
- [PaoloC68/ComfyUI-PuLID-Flux-Chroma (community Chroma port)](https://github.com/PaoloC68/ComfyUI-PuLID-Flux-Chroma)
- [jeankassio/ComfyUI-PuLID-Flux-Chroma-Compatibility-](https://github.com/jeankassio/ComfyUI-PuLID-Flux-Chroma-Compatibility-)
- [aoxo/flux.1dev-abliterated (HuggingFace)](https://huggingface.co/aoxo/flux.1dev-abliterated)
- [Uncensoring Flux.1 Dev: Abliteration (Medium)](https://medium.com/@aloshdenny/uncensoring-flux-1-dev-abliteration-bdeb41c68dff)
- [TencentARC/PhotoMaker (GitHub)](https://github.com/TencentARC/PhotoMaker)
- [InstantX/InstantID (HuggingFace)](https://huggingface.co/InstantX/InstantID)
- [InsightFace license discussion #2469](https://github.com/deepinsight/insightface/issues/2469)
- [InstantID composition-lock (myByways)](https://mybyways.com/blog/consistent-portraits-revisisted-instantid)
- [FLUX.1-Kontext-dev (HuggingFace)](https://huggingface.co/black-forest-labs/FLUX.1-Kontext-dev)
- [XLabs-AI flux-ip-adapter](https://github.com/XLabs-AI/flux-ip-adapter)
- [Consistent Characters - Chroma / IPAdapter / PuLID / ClipVision (Civitai)](https://civitai.com/models/1694024/consistent-characters-face-and-body-nsfw-chroma-ipadapter-pulid-clipvision)
- [OmniGen2 (VectorSpaceLab)](https://vectorspacelab.github.io/OmniGen2/)
- [ConsiStory / StoryDiffusion / CharacterFactory (arXiv 2404.15677)](https://arxiv.org/abs/2404.15677)
- [ReMix: consistent character generation and editing (arXiv 2510.10156)](https://arxiv.org/pdf/2510.10156)
- [StorySync: Training-Free Subject Consistency (arXiv 2508.03735)](https://arxiv.org/pdf/2508.03735)
- [Stable Signature (Meta) invisible watermark](https://ai.meta.com/blog/stable-signature-watermarking-generative-ai/)
- [NVIDIA Pascal Tuning Guide (P100 sm_60)](https://docs.nvidia.com/cuda/pascal-tuning-guide/index.html)
- [FLUX.1 dev abliterated on aimodels.fyi](https://www.aimodels.fyi/models/huggingFace/flux.1dev-abliterated-aoxo)
