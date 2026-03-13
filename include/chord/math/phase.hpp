#pragma once

#include <kfr/all.hpp>

namespace chord::math {

/**
 * @brief State container for continuous phase unwrapping.
 */
struct PhaseUnwrapState {
    float previous_phase{0.0f};
    float wrap_offset{0.0f};
};

/**
 * @brief Unwraps phase continuously across buffers.
 *
 * In signal processing, phase values naturally "wrap" around a circle (like a clock resetting
 * from 12 to 1). This function eliminates those artificial jumps, creating a continuous,
 * smoothly accumulating line of phase over time. This continuous phase is mathematically
 * required before you can perform tasks like calculating the true frequency of a signal.
 *
 * @param in Read-only view of the wrapped input phases.
 * @param out Write view where unwrapped phases will be stored (can be the same as 'in' for
 * in-place).
 * @param state State maintained across buffer calls.
 * @param period The phase wrapping period (defaults to 2*PI).
 */
void unwrap_phase(kfr::univector_ref<const float> in,
                  kfr::univector_ref<float> out,
                  PhaseUnwrapState& state,
                  float period = kfr::c_pi<float, 2>);

/**
 * @brief Computes instantaneous frequency from a continuous phase stream.
 *
 * Frequency is technically the "rate of change" of phase (how fast the clock hand is spinning).
 * This function calculates exactly that: it finds the difference between the current phase
 * and the previous one, giving you the true frequency of the signal at that exact moment.
 * It is commonly used as the core engine for FM demodulation or analyzing radar/sonar Doppler
 * shifts.
 *
 * @note The input to this function MUST be a continuous, unwrapped phase stream.
 *
 * @param unwrapped_phase Read-only view of the continuous unwrapped input phase.
 * @param out Write view where computed frequency values will be stored.
 * @param previous_phase The final phase value from the preceding buffer (updated continuously).
 * @param sample_rate The sample rate for scaling the difference (defaults to 1.0f).
 */
void instantaneous_frequency(kfr::univector_ref<const float> unwrapped_phase,
                             kfr::univector_ref<float> out,
                             float& previous_phase,
                             float sample_rate = 1.0f);

/**
 * @brief Wraps a phase value into the canonical range [-pi, pi).
 *
 * Phase values can accumulate indefinitely in algorithms like PLLs or NCOs. Without periodic
 * wrapping, the floating-point value grows until it loses precision. This function folds any
 * radian value back into [-pi, pi), which is the standard conventional range used in virtually
 * all DSP systems. It is a stateless scalar operation and can be applied freely to any
 * intermediate phase variable to prevent numeric drift over time.
 *
 * @param phase The input phase in radians (any value).
 * @return The equivalent phase wrapped into the range [-pi, pi).
 */
float wrap_phase(float phase);

}  // namespace chord::math
