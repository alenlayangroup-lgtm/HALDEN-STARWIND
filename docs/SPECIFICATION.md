# HALDEN STARWIND — Product Specification 1.0

## Identity
Premium, balanced, airy and deep hybrid Hall reverb for macOS AU/VST3.

## Signal path
Input → optional De-Esser → optional Chorus → Hybrid Hall → BREEZE / STARDUST / SILK TAIL → optional Post EQ → Output.

## Signature modules
- **BREEZE:** restrained evolving modulation, slow and organic stereo motion.
- **STARDUST:** subtle spectral shimmer with controlled high-frequency energy.
- **SILK TAIL:** dense diffusion, smooth frequency-dependent decay and low metallic coloration.

## Core Hall
Early reflections + diffusion network + feedback delay network (FDN), with predelay, decay, size, width, diffusion, damping and modulation controls.

## Special modes
- **INFINITE:** controlled extended decay.
- **FREEZE:** capture and sustain the reverb state without unstable feedback or clicks.

## Processing blocks
- De-Esser before reverb, independently bypassable.
- Chorus before reverb, independently bypassable.
- Post EQ after reverb, independently bypassable, with HPF, shelves and parametric bands.
- Wet/dry, output gain, width, optional ducking and mono monitoring.

## Engineering requirements
Real-time safe audio callback; no allocations or blocking locks in processBlock; smoothed automation; state/preset recall; mono/stereo support; sample-rate correctness; finite output; measured CPU and latency; AU/VST3 validation.

## Release gates
The current prototype is only a build foundation. A release candidate requires the custom Hall and all advertised modules to be implemented, tested and auditioned. No unlicensed third-party assets or copied commercial algorithms.
