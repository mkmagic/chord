# chord
Zero-copy, auto-vectorized DSP primitives for software-defined radio and communications, built natively on top of the KFR C++ SIMD library.

## Library Design

### Functions API
`chord` is built to maximize the performance of real-time DSP through KFR's SIMD engine while delivering exceptional ergonomics for modern C++ developers. To achieve this balance, all processing functions are standardized to accept `kfr::univector_ref<const T>` for input views, and `kfr::univector_ref<T>` (or return an owning `kfr::univector`) for outputs.

This architectural decision yields profound benefits for cross-system interoperability:
- **Zero-Copy Views:** The library performs operations directly on audio buffers or data blocks without forcing reallocation, provided the caller ensures appropriate contiguous alignment.
- **Native `std::span` Support:** Thanks to C++ duck typing and KFR's underlying template constructors, you do not need to explicitly bridge standard containers. Any type that implements `.data()` and `.size()`—such as `std::span`, `std::vector`, or `std::array`—can be passed *directly* into `chord` functions seamlessly.
- **Custom Hardware Containers:** If you have custom driver buffers (e.g., SDR stream block definitions) that just expose `.data()` and `.size()`, `chord` will consume them equally natively. 
- **Maximum KFR Advantage:** Users who already leverage KFR locally and pass native `kfr::univector` allocations experience zero conversion overhead, dropping directly into optimal aligned instructions.

## Roadmap

To support real-time communications, satellite tracking, and RF cyber applications, `chord` will build upon KFR's foundational math to implement the following high-level DSP blocks and helpers:

### 1. Core Math & RF Helpers
- Phase Unwrapping (stateful stream processing)
- Instantaneous Frequency Calculation (differentiating phase vectors)
- Zero Crossing Detector (for clock recovery and simple FM)
- Fast Moving Average / Moving Variance
- Complex Signal Operations (conjugate multiply, magnitude squared)

### 2. Filtering & Signal Conditioning
- Stateful Streaming FIR / IIR Filters (wrapping KFR's filter primitives for continuous streams)
- Polyphase Decimators and Interpolators (for efficient sample rate conversion)
- Automatic Gain Control (AGC) with configurable attack/decay
- Numerically Controlled Oscillators (NCO) for complex frequency translation / shifting

### 3. Synchronization & Recovery
- Phase Locked Loops (PLL)
- Costas Loops (for BPSK / QPSK carrier tracking)
- Timing Recovery algorithms (Gardner, Mueller and Muller)
- Frame Synchronization (Correlators / Access Code detection)

### 4. Demodulators & Modulators
- **Analog:** FM (Quadrature Demod / Discriminator), AM, SSB
- **Digital Baseband:** FSK, PSK (BPSK, QPSK, 8PSK), QAM
