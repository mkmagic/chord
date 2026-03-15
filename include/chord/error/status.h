#pragma once

namespace chord {

/**
 * @brief Status codes for chord APIs that can fail.
 *
 * Bins cover common failure reasons across the library, such as invalid inputs,
 * division-by-zero risks, missing/short buffers, and workspace mismatches.
 */
enum class Status {
    /// Operation completed successfully.
    OK = 0,
    /// One or more parameters are invalid (range, enum, or unsupported combination).
    INVALID_PARAM,
    /// Operation would divide by zero or use an undefined scale.
    DIVIDE_BY_ZERO,
    /// Output buffer is too small for requested operation.
    OUTPUT_TOO_SMALL,
    /// Input buffer is empty or too short for requested operation.
    INPUT_TOO_SMALL,
    /// Required workspace or plan does not match requested configuration.
    WORKSPACE_MISMATCH,
};

}  // namespace chord
