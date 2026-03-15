<p align="center">
  <img src="docs/assets/banner.png" alt="C.H.O.R.D. Banner" width="100%">
</p>

[![x86 GCC](https://github.com/mkmagic/chord/actions/workflows/main-x86-gcc.yml/badge.svg?branch=main&event=push)](https://github.com/mkmagic/chord/actions/workflows/main-x86-gcc.yml) [![x86 Clang](https://github.com/mkmagic/chord/actions/workflows/main-x86-clang.yml/badge.svg?branch=main&event=push)](https://github.com/mkmagic/chord/actions/workflows/main-x86-clang.yml) [![ARM Clang](https://github.com/mkmagic/chord/actions/workflows/main-arm-clang.yml/badge.svg?branch=main&event=push)](https://github.com/mkmagic/chord/actions/workflows/main-arm-clang.yml) [![codecov](https://codecov.io/gh/mkmagic/chord/branch/main/graph/badge.svg)](https://codecov.io/gh/mkmagic/chord)

C.H.O.R.D. is a zero-copy, auto-vectorized DSP primitives library for software-defined radio and communications, built natively on top of the KFR C++ SIMD library.

## Library Design

### Functions API
`chord` is built to maximize the performance of real-time DSP through KFR's SIMD engine while delivering exceptional ergonomics for modern C++ developers. To achieve this balance, all processing functions are standardized to accept `kfr::univector_ref<const T>` for input views, and `kfr::univector_ref<T>` (or return an owning `kfr::univector`) for outputs.

This architectural decision yields profound benefits for cross-system interoperability:
- **Zero-Copy Views:** The library performs operations directly on audio buffers or data blocks without forcing reallocation, provided the caller ensures appropriate contiguous alignment.
- **Native `std::span` Support:** Thanks to C++ duck typing and KFR's underlying template constructors, you do not need to explicitly bridge standard containers. Any type that implements `.data()` and `.size()`—such as `std::span`, `std::vector`, or `std::array`—can be passed *directly* into `chord` functions seamlessly.
- **Custom Hardware Containers:** If you have custom driver buffers (e.g., SDR stream block definitions) that just expose `.data()` and `.size()`, `chord` will consume them equally natively. 
- **Maximum KFR Advantage:** Users who already leverage KFR locally and pass native `kfr::univector` allocations experience zero conversion overhead, dropping directly into optimal aligned instructions.

## Roadmap

To support real-time communications, satellite tracking, and RF cyber applications, `chord` avoids monolithic end-to-end systems. Instead, it provides the blazing-fast, stateful DSP **primitives** that empower users to build complex algorithms (like Carrier Recovery or Baudrate Estimation) themselves.

### 1. Core Math & RF Helpers
- Phase Unwrapping (stateful stream processing)
- Instantaneous Frequency Calculation (differentiating phase vectors)
- Zero Crossing Detector (for clock recovery and simple FM)
- Fast Moving Average / Moving Variance
- FM Baseband Demodulator (Quadrature Discriminator)

### 2. Signal Generation & Translation
- **Numerically Controlled Oscillators (NCO):** Fast complex sine generation with phase accumulation for frequency shifting and carrier recovery.
- **Fast Math Approximations:** Ultra-fast SIMD approximations for `sinc()`, `atan2()`, and trigonometric functions used in deep inner loops.

### 3. Delays & Resampling
- **Fractional Delay Lines:** Stateful buffers with linear or cubic interpolation for symbol timing recovery algorithms (like Gardner/Mueller & Muller).
- **Polyphase Filterbanks:** Efficient PFB channelizers and Resamplers for drastic sample rate reductions.
- **Stateful Streaming Filters:** Seamless wrappers for KFR's FIR/IIR topologies maintaining state across buffer boundaries.

### 4. Synchronization Primitives
- **Phase Detectors:** Phase Error and Frequency Error detectors for driving external PLLs.
- **Automatic Gain Control (AGC):** Fast feed-forward and feed-backward power tracking with configurable attack/decay rates.
- **Correlators:** High-speed sliding window correlators for Access Code and Frame Sync detection.
