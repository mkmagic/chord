#pragma once

#include <kfr/all.hpp>
#include <vector>

namespace chord::dsp {

/**
 * @brief Specifies the interpolation algorithm used for fractional delay reconstruction.
 */
enum class InterpolationType {
    Linear, ///< 2-point interpolation. Fast, but introduces amplitude roll-off and noise.
    Cubic   ///< 4-point Hermite interpolation. Higher fidelity at slightly higher CPU cost.
};

/**
 * @brief State container for a fractional delay line.
 *
 * This struct maintains a circular buffer of history samples. It is used to reconstruct
 * signal values at non-integer sample offsets via interpolation.
 */
struct FractionalDelayState {
    kfr::univector<float> buffer; ///< Circular buffer of history samples.
    size_t write_idx{0};           ///< Current position to write the next input sample.
    size_t size{0};               ///< Total size of the circular buffer.
};

/**
 * @brief Factory function to create and pre-allocate a FractionalDelayState.
 *
 * @param max_delay The maximum number of samples of delay this state can support.
 *                  The actual buffer size will be slightly larger to accommodate
 *                  interpolation kernels (e.g., 4 points for cubic).
 * @return A fully initialized FractionalDelayState.
 */
FractionalDelayState make_fractional_delay_state(size_t max_delay);

/**
 * @brief Processes a block of samples through a fractional delay line.
 *
 * This function supports per-sample variable delay. For each input sample, it is first
 * written into the history buffer, and then an output sample is reconstructed at the
 * requested delay offset. A delay of 0.0 returns the current input sample exactly.
 *
 * @note This implementation is preferred over `kfr::fracdelay` because it supports
 *       per-sample variable delays (essential for timing recovery loops) and a 
 *       full range of delays up to `max_delay`, whereas the KFR built-in is 
 *       restricted to a fixed 0..1 range per block.
 *
 * @note This implementation is preferred over `kfr::fracdelay` because it supports
 *       per-sample variable delays (essential for timing recovery loops) and a 
 *       full range of delays up to `max_delay`, whereas the KFR built-in is 
 *       restricted to a fixed 0..1 range per block.
 *
 * @param in Read-only view of input samples.
 * @param out Write view for delayed output samples.
 * @param delays View of per-sample delay offsets (in fractional samples).
 *               Values should be in range [0, max_delay].
 * @param state State maintained across buffer calls.
 * @param interp The interpolation method to use (defaults to Linear).
 */
void fractional_delay(kfr::univector_ref<const float> in,
                      kfr::univector_ref<float> out,
                      kfr::univector_ref<const float> delays,
                      FractionalDelayState& state,
                      InterpolationType interp = InterpolationType::Linear);

} // namespace chord::dsp
