# Sound-effect + TTS model scouting for ac9

Constraints assumed: 2x Tesla P100 (16 GB, sm_60), CUDA UVA available,
existing runtime shells out to purpose-built C++ binaries (`sd-cli` for
image, `llama.cpp` for LLM). Same shape wanted for audio.

---

## 1. Sound-effect / general-audio generation

| Model | License | Size | HF page | ggml/gguf | Abliterated? |
|---|---|---|---|---|---|
| **Bark (Suno)** | MIT (weights + code) | ~1.0 B (large), ~400 M (small) | `suno/bark`, `suno/bark-small` | No official GGUF. Community `bark.cpp` port exists (ggerganov-adjacent, mostly stale). | N/A - model itself is unrestricted, but Suno embeds an inaudible watermark in output. No abliterated variant exists or is needed. |
| **AudioLDM** | CC-BY-NC-SA 4.0 | 1.69-3.37 GB weights | `cvssp/audioldm`, `cvssp/audioldm-s-full-v2` | No GGUF. Latent-diffusion in diffusers only. | N/A. |
| **AudioLDM 2** | CC-BY-NC-SA 4.0 | 1.1 B (base), 1.5 B (large) | `cvssp/audioldm2`, `cvssp/audioldm2-large` | No GGUF. | N/A. |
| **Stable Audio Open 1.0** | Stability AI Community License (free for orgs <$1M ARR, research OK) | 1.21 B params, 47 s stereo @ 44.1 kHz | `stabilityai/stable-audio-open-1.0` (+ `-small` for 11 s) | No GGUF. `diffusers` StableAudioPipeline only. | N/A. |
| **MusicGen (Meta)** | CC-BY-NC 4.0 weights / MIT code | 300 M / 1.5 B / 3.3 B | `facebook/musicgen-small|medium|large` | No first-party GGUF. Historically had a ggml demo in the audiocraft/ggml era; not maintained. | N/A. |
| **MAGNeT (Meta)** | CC-BY-NC 4.0 / MIT code | `magnet-small-10secs`, `medium-30secs`, `audio-magnet-*` | `facebook/magnet-*`, `facebook/audio-magnet-*` | No GGUF. Runs via audiocraft. Faster than MusicGen (non-autoregressive). | N/A. |

**License takeaway.** Of the six, only **Bark (MIT)** and **Stable
Audio Open (SAI Community, <$1M ARR)** are usable without a
non-commercial trip-wire. AudioLDM/MusicGen/MAGNeT are all CC-BY-NC -
fine for a solo dev experiment, radioactive the moment ac9 is shipped.

**ggml takeaway.** None of the six has an actively-maintained,
first-party GGUF. Any deployment today means shelling out to a Python
audiocraft/diffusers process. That's the same shape as the current
`sd-cli` bridge (external binary + PNG on disk), so integration is
tractable - it just isn't llama.cpp-native.

---

## 2. Text-to-speech

| Model | License | Size | Quality | ggml/gguf | "Uncensored"? |
|---|---|---|---|---|---|
| **Coqui XTTS-v2** | CPML (**non-commercial**, orphaned - Coqui Inc. shut down Jan 2024, no one to sell you a commercial license) | ~500 M | Excellent, 17-lang voice cloning | No GGUF. PyTorch only. | Model has no content filter, but the loader prompts for EULA acceptance the first time. Bypass with `COQUI_TOS_AGREED=1`. Not a refusal - a click-through. |
| **Piper** | MIT (original `rhasspy/piper`); GPL-3 for the new `OHF-Voice/piper1-gpl` fork | 20-60 MB per voice (VITS/ONNX) | Good, robotic vs. modern | ONNX only (not GGML), but genuine C++. ~10x realtime on desktop CPU, realtime on RPi 5. | No refusal. Ships hundreds of prebuilt voices. |
| **Kokoro-82M** | **Apache-2.0** | 82 M | Punches far above weight class; ranks near XTTS in blind MOS despite ~6x smaller | **YES.** GGUF at `cstr/kokoro-82m-GGUF` and `mmwillet2/Kokoro_GGUF`. F16 = 202 MB, Q8_0/Q5_0/Q4_0 all available. Runs via **`mmwillet/TTS.cpp`** on GGML. | No refusal at all. Zero content filtering. |
| **Bark (Suno)** | MIT | ~1 B | Great for expressive/non-speech (laughs, sighs, sound effects mid-sentence). Multi-lingual. | No official GGUF. `bark.cpp` community port exists but is stale. | No refusal in the model. Watermark embedded in output audio. |
| **Silero** | Various (weights permissive, distribution model unusual - via torch.hub) | Small (tens of MB) | Decent, robotic | No GGUF. PyTorch/ONNX. | No refusal. |
| **F5-TTS** | CC-BY-NC 4.0 base; **OpenF5-TTS-Base** is Apache-2.0 (retrain on permissive data) | ~330 M DiT + 13 M vocoder | Excellent zero-shot cloning, flow-matching | GGUF exists at `cstr/f5-tts-GGUF` (MIT C++ port). Flow-matching means **F16 only**; Q8/Q5/Q4 produce unintelligible output because quantization error compounds through the flow. | No refusal. |
| **StyleTTS2** | MIT (code) | 100 M-class | Strong; Kokoro is architecturally a distilled StyleTTS2 | No official GGUF (Kokoro is your effective GGUF path). | No refusal. |
| **MetaVoice** | Apache-2.0 | ~1.2 B | Solid clone quality, but org is basically defunct - no updates since early 2024 | No GGUF. | No refusal. |
| **OpenVoice v2 (MyShell + MIT)** | **MIT** (since Apr 2024) | ~2 GB total pipeline | Good tone/emotion transfer, requires a base speaker | No GGUF. PyTorch only. | No refusal. |
| **Chatterbox (Resemble.ai)** | **MIT** | 0.5 B (Llama backbone) | 65.3% preferred vs. ElevenLabs in vendor blind test. Best-in-class OSS as of mid-2026. 25 languages in v3. | No GGUF yet, but Llama-backbone means one is plausible. | No refusal, but **PerTh watermark is embedded by default in every self-hosted output**. |

---

## 3. Abliteration reality check

**Text LLMs get abliterated because they refuse.** They have a
learned "I can't help with that" direction in activation space that
gets zeroed out. That model of refusal doesn't apply to the audio
stack in any of the ways it applies to a chat model:

- **TTS models don't judge content.** They map phonemes to
  spectrograms. A TTS model will happily read a bomb recipe, slurs, or
  a suicide note in the same neutral voice it uses for the weather.
  There is nothing to abliterate.
- **Sound-fx / music models don't judge content either.** Stable Audio,
  MusicGen, AudioLDM, MAGNeT, Bark's non-verbal channel - none of them
  have a refusal circuit. They may be trained on data that biases them
  away from certain sounds (Meta redacted vocals from MusicGen's
  training set), but that's a data-side gap, not a refusal.
- **What "refusal" actually looks like in audio land:**
  1. **Loader-level EULA click-throughs** (Coqui XTTS's TOS prompt,
     Stability's Community License gating). Solved by env var or reading
     the license - not by weight surgery.
  2. **Voice-cloning safeguards** - Coqui and Chatterbox refuse to clone
     protected voices in their hosted product. Local weights don't
     enforce this. Nothing to abliterate.
  3. **Embedded watermarks** - Suno Bark and Chatterbox v3 stamp an
     inaudible signature into output. Stripping these is a signal-
     processing problem, not an abliteration problem, and doing so
     violates their EULAs.

**Recommendation:** do not chase abliterated audio models. They don't
exist in any meaningful quantity because there is no demand - the
underlying models don't refuse. Time better spent on license fit
(Apache-2.0 / MIT) and watermark awareness.

---

## 4. P100 (sm_60) fit assessment

- **llama.cpp CUDA on P100** works today (~60 t/s on 7-13B class
  models; ac9 already runs coder-30B-A3B on these cards via UVA). One
  known caveat: `flash-attn` kernels don't compile for sm_60. Turn it
  off. All other CUDA kernels are fine.
- **whisper.cpp CUDA on P100** works. sm_60 has been the floor of the
  ggml CUDA backend for years.
- **TTS.cpp status (mmwillet/TTS.cpp, the ggml TTS runtime):**
  - Supports Kokoro, Parler-TTS, Dia, Orpheus.
  - **CUDA is explicitly not implemented** - matrix marks CUDA as "no"
    for all four models. CPU + Metal only. Project self-describes as a
    "proof of concept," best-supported on macOS.
  - Kokoro is 82 M params - CPU inference is easily realtime even
    without GPU. The P100 is wasted here.
- **F5-TTS GGUF (`cstr/f5-tts-GGUF`)** runs on CPU + GPU via a C++
  port, but F16 only. Q8/Q5/Q4 quants produce garbled audio due to
  flow-matching precision sensitivity.
- **Bark on P100** works fine in PyTorch (compute-capability floor for
  modern PyTorch wheels is sm_50-60). Not GGML-native.
- **Stable Audio Open on P100** works in PyTorch via diffusers.
  1.21 B params + T5 encoder - comfortably under 16 GB with UVA as
  backstop.
- **MusicGen small (300 M)** on P100: trivial. Medium (1.5 B): fine.

**Sm_60 blockers:** none from a runtime-support angle. The blockers
are format (nothing is GGUF-native yet outside Kokoro), and the fact
that the one purpose-built GGML TTS runtime doesn't have CUDA wired in.

---

## 5. Recommendation

### Top sound-fx pick: **Stable Audio Open 1.0**
- Best quality of the commercially-usable set (Bark is the only
  MIT-licensed alternative and it's noticeably weaker for pure sound
  effects and music).
- License permits ac9's likely use case (solo/small-org, <$1M ARR).
- No GGUF - integration means a Python `sd-audio-cli`-shaped bridge
  (venv + `diffusers` + `stable-audio-tools`), same pattern as
  `sd-cli` for images. **Integration effort: 3/5**, ~350 LoC to mirror
  `image_generator.{hpp,cpp}` (293 + 35 LoC today).
- Fallback if Stability license is a problem: **Bark (small)**. MIT,
  ships sound effects and speech in one model, ~400 M params, needs a
  PyTorch bridge but is watermark-tagged.

### Top TTS pick: **Kokoro-82M via TTS.cpp**
- Apache-2.0, no license traps.
- GGUF-native (`mmwillet2/Kokoro_GGUF`, F16 = 202 MB).
- Runs through `mmwillet/TTS.cpp` - a GGML-backed C++ binary, exact
  same integration shape as `sd-cli`. Shell out, get a WAV, done.
- 82 M params runs realtime on CPU; you don't need the P100 for this
  and it's fine that TTS.cpp lacks CUDA today.
- **Integration effort: 2/5**, ~250 LoC for a
  `modules/1627_tts_generator/` module that mirrors
  `1624_image_generator/` (init/status/generate, shell-launch a
  `tts-cli` binary, return WAV path).
- Fallback if voice cloning is desired: **Chatterbox** (MIT, 0.5 B,
  best-in-class quality) once someone lands a GGUF port - today it's
  PyTorch-only, effort jumps to 3-4/5 with a Python bridge, and
  outputs carry Resemble's PerTh watermark.

### Skip list
- **XTTS-v2** - orphaned license, no one to license commercial use
  from. Great model, do not build on it.
- **AudioLDM / AudioLDM2 / MusicGen / MAGNeT** - CC-BY-NC. Fine for a
  local demo, poison for anything shipped.
- **Piper (new fork)** - GPL-3 fork; original MIT copy is fine but
  archived. Quality lags Kokoro.
- **F5-TTS quantized** - flow-matching + int quantization is
  incompatible today. Use F16 if at all.
