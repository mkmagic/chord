# Copilot Instructions for `chord`

## What this project is
- `chord` is a C++20 DSP primitives library built on top of KFR (`README.md`, `CMakeLists.txt`).
- The core design goal is zero-copy, SIMD-friendly processing for streaming SDR/comms workloads.
- Public API lives in `include/chord/*`; implementations live in `src/*`; tests mirror modules in `tests/*`.

## Module boundaries and data flow
- `chord::math`: scalar/stream math helpers and stateful primitives (`phase`, `stats`, `zero_cross`, `correlation`).
- `chord::dsp`: signal-generation/analysis primitives (`nco`, `pulse_design`, `spectrum`).
- `chord::demod`: demodulator primitives (`fm`).
- Stateful processing is explicit via state structs passed by reference across calls (e.g., `NcoState`, `PhaseUnwrapState`, `FmDemodulatorState`).

## API conventions used here (follow these)
- Prefer `kfr::univector_ref<const T>` inputs and `kfr::univector_ref<T>` outputs for non-owning buffer views.
- Keep streaming continuity via caller-owned state structs; do not hide cross-buffer state in globals/statics.
- Handle invalid/short buffers with early return (see `src/dsp/spectrum.cpp`, `src/dsp/pulse_design.cpp`).
- Use KFR expression/slice operations for vectorized paths, and only scalar loops where state coupling requires it.
- Boundary-first pattern is common: handle index `0` with previous-state manually, then vectorize remaining slices (see `src/demod/fm.cpp`).

## Build, test, and coverage workflow
- Configure with CMake and build the `chord` target plus optional tests/examples.
- Typical local flow:
  - `cmake -B build -G Ninja -DCHORD_BUILD_TESTS=ON`
  - `cmake --build build`
  - `ctest --test-dir build --output-on-failure --no-tests=error`
- Coverage path is wired through `ENABLE_COVERAGE=ON` (GCC/Clang only) and `gcovr` in CI (`.github/workflows/ci.yml`).
- CI matrix validates both Clang and GCC on Ubuntu (x64 + ARM), so avoid compiler-specific assumptions.

## When adding/changing code
- Add new implementation files to `src/CMakeLists.txt` (`target_sources(chord ...)`).
- Add matching tests to `tests/CMakeLists.txt` (`add_executable(chord_tests ...)`).
- Keep tests close to module behavior and streaming semantics (split-buffer continuity checks are common, e.g. `tests/dsp/test_nco.cpp`).
- Prefer `kfr::univector` in tests and pass `.ref()`/slices to public APIs.

## Integration points and dependencies
- KFR is fetched via `FetchContent` and linked as `kfr`, `kfr_dsp`, `kfr_dft`, `kfr_io`.
- GTest is fetched only when tests are enabled at top-level.
- `examples/run_examples.sh` expects a configured build tree because it installs Python deps from `build/_deps/kfr-src/requirements.txt` and runs `build/examples/plot_pulse`.
- `TEST_DATA_DIR` is provided to tests via compile definition in `tests/CMakeLists.txt`.
