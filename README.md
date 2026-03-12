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
