#pragma once

#include <kfr/all.hpp>

#include <cmath>

namespace chord::dsp {

/**
 * @brief State container for Numerically Controlled Oscillators (NCO).
 */
struct NcoState {
    float phase{0.0f};  ///< Accumulated phase in radians, bounded [-pi, pi).
};

/**
 * @brief Generates a continuous phase vector from a given frequency.
 *
 * This function accumulates phase across buffer boundaries, allowing for continuous
 * tracking and signal generation without phase discontinuities. The phase is
 * always wrapped between [-pi, pi] in the State tracker to prevent precision loss.
 *
 * @param out_phase Write view for the accumulated radian phase values.
 * @param state State maintained across buffer calls to ensure phase continuity.
 * @param frequency The desired frequency to generate (in Hz).
 * @param sample_rate The sampling rate of the system (in Hz).
 */
void nco_generate(kfr::univector_ref<float> out_phase,
                  NcoState& state,
                  float frequency,
                  float sample_rate);

/**
 * @brief Generates a complex exponential signal (e^(j*phase)) from a given frequency.
 *
 * This is the core of a Numerically Controlled Oscillator (NCO). It generates a clean,
 * continuous complex sine wave that can be used directly for frequency shifting or
 * as the local oscillator in a generic Mixer block.
 *
 * @param out_iq Write view for the complex exponential signal.
 * @param state State maintained across buffer calls to ensure phase continuity.
 * @param frequency The desired frequency to generate (in Hz).
 * @param sample_rate The sampling rate of the system (in Hz).
 * @param gain Optional amplitude scaling factor applied to each IQ sample (default 1.0).
 *             The multiplication is fused into the KFR SIMD expression at no extra cost.
 */
void nco_generate_complex(kfr::univector_ref<kfr::complex<float>> out_iq,
                          NcoState& state,
                          float frequency,
                          float sample_rate,
                          float gain = 1.0f);

}  // namespace chord::dsp
