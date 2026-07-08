---
title: Image editing
parent: Features
nav_order: 3
---

# Image editing
{: .no_toc }

The `image_edit` tool is `ac9`'s img2img editor: a Photoshop / GIMP
class module (`advanced_raster_image_editor_photoshop_gimp_class`)
that takes an existing PNG and an instruction ("make the sky
stormier," "give the character a red hat") and re-diffuses through
Chroma1-HD with an init image and a controlled denoise strength.

## Flow

1. The tool router picks `image_edit` when the prompt reads as an
   edit rather than a fresh generation.
2. The [image resolver](image-resolver.html) figures out **which**
   image the user meant — by filename in the prompt, by session
   history, or by vision description of every image in the project
   tree.
3. The editor runs the underlying `image_generator::run` in img2img
   mode. `strength` defaults to `0.55` — high enough that the edit
   is meaningful, low enough that the subject stays recognisable.
   The operator can override with `strength=` in the prompt.
4. The output PNG is written next to the source with a suffixed
   basename so the original is preserved.

## Why it exists

The editor used to fail with "no input image to edit" whenever the
session had no prior gen or edit turn. That put `ac9` in the
position of giving up on a request the user was perfectly capable
of specifying. The resolver + editor pair together removes that
failure mode — the pipeline never gives up as long as there is
anything in the project tree to look at.

## Coming soon

- Region masks (paint the area to be edited, leave the rest untouched).
- Multi-step edit chains that keep a rolling seed for identity
  preservation.
- LoRA-aware edits so a canonical character stays on-model through
  every revision.
