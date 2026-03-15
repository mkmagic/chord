#pragma once

#include <chord/error/status.h>

#include <kfr/all.hpp>

#include <cstdint>

namespace chord::math {

/**
 * @brief State container for zero crossing detection.
 */
struct ZeroCrossState {
    float previous_sample{0.0f};
    bool initialized{false};
};

/**
 * @brief Populates a mask where 1 indicates a zero crossing occurred.
 *
 * This function scans an audio signal and places a '1' exactly where the wave crosses the
 * horizontal zero line (changing from positive to negative, or vice versa). Counting these
 * zero crossings is one of the fastest and simplest ways to estimate a signal's original pitch
 * or recover digital clocks without needing complex Fourier transforms.
 *
 * @param in Read-only view of the input signal.
 * @param out_mask Write view where the boolean (uint8_t) mask will be stored.
 * @param state State maintained across buffer calls to catch boundary crossings.
 * @return Status code indicating success or failure reason.
 */
Status detect_zero_crossings(kfr::univector_ref<const float> in,
                             kfr::univector_ref<uint8_t> out_mask,
                             ZeroCrossState& state);

}  // namespace chord::math
