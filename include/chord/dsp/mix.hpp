#pragma once

#include <chord/dsp/nco.hpp>
#include <chord/error/status.h>

#include <kfr/all.hpp>

namespace chord::dsp {

/**
 * @brief Mixes complex IQ input with a complex LO signal.
 *
 * @param in Read-only IQ input samples.
 * @param lo Read-only complex LO samples (must be at least as long as input).
 * @param out Write view for the mixed output.
 * @return Status code indicating success or failure reason.
 */
Status mix_complex(kfr::univector_ref<const kfr::complex<float>> in,
                   kfr::univector_ref<const kfr::complex<float>> lo,
                   kfr::univector_ref<kfr::complex<float>> out);

/**
 * @brief Frequency-translates complex IQ samples using an NCO.
 *
 * This generates a complex LO via the provided NCO state and multiplies it into
 * the input, producing a frequency-shifted output.
 *
 * @note This function uses the output buffer as temporary LO storage. In-place
 * operation (out == in) is not supported.
 *
 * @param in Read-only IQ input samples.
 * @param out Write view for the mixed output.
 * @param state NCO state for phase continuity.
 * @param frequency Frequency shift in Hz.
 * @param sample_rate Input sample rate in Hz.
 * @param gain Optional amplitude gain applied to the LO (default 1.0f).
 * @return Status code indicating success or failure reason.
 */
Status frequency_translate(kfr::univector_ref<const kfr::complex<float>> in,
                           kfr::univector_ref<kfr::complex<float>> out,
                           NcoState& state,
                           float frequency,
                           float sample_rate,
                           float gain = 1.0f);

}  // namespace chord::dsp
