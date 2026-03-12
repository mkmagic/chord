---
trigger: always_on
---

# C++ DSP Library Development Rules

You are an expert C++ developer building a real-time Digital Signal Processing (DSP) library. Your generated code, architectural suggestions, and refactoring tasks must strictly adhere to the following guidelines:

## 1. Architecture & API Design
* **Modern C++ API:** Design a clean, developer-oriented API utilizing modern C++ idioms.
* **CMake Integration:** Ensure the library can be effortlessly integrated into external codebases using standard `add_subdirectory` or `FetchContent` commands. 
* **Folder Structure:** Maintain a logical directory structure that cleanly separates core algorithms, utilities, and testing configurations.

## 2. DSP & Performance 
* **Real-time Constraints:** All algorithms and data structures must be optimized for deterministic, real-time DSP performance. Avoid hidden allocations, locks, or unbounded operations in the hot path.
* **Leverage KFR:** Do not reinvent the wheel. Always prefer and build upon existing KFR implementations for SIMD-accelerated math, complex operations, and fundamental DSP building blocks.
* **API Standard:** All mathematical and processing functions must accept `kfr::univector_ref<const T>` for input spans, and `kfr::univector_ref<T>` (or return an owning `kfr::univector`) for outputs. This strictly enforces zero-allocation, pre-aligned memory views at the boundary while maintaining interoperability with standard C++ `std::span`.
* **Cross-compatibility:** Ensure all vectorized math and cross-compilation targets are cleanly handled through the KFR backend.

## 3. Code Organization & DRY Principles
* **Separation of Concerns:** * Declarations must reside exclusively in `.h` or `.hpp` files.
  * Implementations must reside in `.cpp` files.
  * For templates, strictly use the `.tpp` convention. Include the `.tpp` file at the very bottom of the corresponding header file (`#include "filename.tpp"`).
* **Modularity:** Do not write repetitive code. Aggressively separate duplicated logic into specialized, reusable helper functions.
* Always make sure the code is linted and formatted using the project's clang-format file.

## 4. Error Handling
* **Modern Error Types:** Avoid traditional C-style return codes or throwing exceptions for expected algorithmic failures. Use `std::optional` for operations that might simply yield no value, and `std::expected` for functions that yield a value or a distinct error state.

## 5. Documentation
* **Doxygen Standards:** Every function, struct, and class must be fully documented using Doxygen comments. This explicitly includes detailed descriptions for all `@param` arguments and `@return` types. 
  * **Clarity:** The `@brief` section must contain a clear, high-level paragraph explaining what the function conceptually does and why it is useful. It should be written so that software developers who are *not* DSP experts can easily understand its purpose and applications.